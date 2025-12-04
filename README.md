# Resources
- [The-GNU-Allocator](https://www.gnu.org/software/libc/manual/html_node/The-GNU-Allocator.html)
- [Once upon a free()](https://phrack.org/issues/57/9)

# LLDB
`settings set target.env-vars LD_PRELOAD=/home/yboudoui/Documents/malloc/libft_malloc.so LANG=C`

# BUG
You’re crashing inside glibc during _nl_expand_alias() → read_alias_file() → qsort_r() → alias_compare().
This has nothing to do with ls itself — it means your libft_malloc.so is breaking glibc’s internal memory usage.

When you run lldb -- ls -la, LLDB loads ls in a different environment than when you run plain ls, so your malloc library is exercised in a different code path. That path loads localization alias files (/usr/share/locale/locale.alias), parses them, stores strings, and uses qsort_r on them.

Your custom malloc is almost certainly corrupting memory.

✅ Why glibc crashes inside alias_compare()

alias_compare() begins with:

mov (%rsi), %rsi
mov (%rdi), %rdi


Meaning: rsi and rdi should each point to a valid char* inside an array of pointers.

But glibc received invalid pointers → your malloc implementation is returning bad allocation results or corrupting freed blocks.

When qsort() dereferences them, you get a segfault.

🎯 Root causes in custom-malloc projects (common)

You return misaligned pointers
glibc requires 8–16 byte alignment.

Your malloc returns memory inside internal metadata

You overwrite memory when writing metadata

Your free() merges/extends blocks incorrectly → double-free–like corruption

You do not handle realloc correctly

You hook malloc/free but do not hook calloc() or realloc()
→ glibc calls those and expects correct zeroing & semantics

Your library runs too early
LD_PRELOAD malloc triggers before your constructors have run, during glibc initialization.

🔥 Important clue

You said:

but with ls only it works

Because when you run:

ls


glibc does not load localization aliases — it uses the default C locale.

But when you run:

lldb -- ls -la


your environment likely sets:

LANG=fr_FR.UTF-8
LC_ALL=…


so glibc loads its locale alias database → lots of mallocs, qsorts, and string operations.

Meaning: your malloc breaks under heavier use.

✅ Confirm the difference

Try:

LANG=C ls -la


You will probably find it does not crash.

Try:

LANG=fr_FR.UTF-8 ls -la


That might crash, same as inside LLDB.

If it crashes only with non-C locale: your malloc implementation is faulty.

🧪 How to debug your custom malloc

Instrument your malloc:

void *malloc(size_t s) {
    void *p = real_malloc(s);
    fprintf(stderr, "[malloc] %zu -> %p\n", s, p);
    return p;
}


And free:

void free(void *p) {
    fprintf(stderr, "[free] %p\n", p);
    real_free(p);
}


Compare against LD_PRELOAD=/usr/lib/libc_malloc_debug.so to understand normal behavior.

🧠 The real fix

Debug your allocator’s:

block splitting

coalescing

header/footer placement

pointer alignment

size rounding (8/16 bytes)

realloc edge cases

calloc zeroing

If you want, paste your malloc / free / realloc code and I’ll identify the exact bug — it's usually a 3-line fix.

You said:

This **has nothing to do with ls itself** — it means your `libft_malloc.so` is breaking glibc’s internal memory usage. When you run **`lldb -- ls -la`**, LLDB loads `ls` in a _different environment_ than when you run plain `ls`, so your malloc library is exercised in a different code path. That path loads _localization alias files_ (`/usr/share/locale/locale.alias`), parses them, stores strings, and uses `qsort_r` on them. Your custom malloc is almost certainly corrupting memory.

why? I mean it's not clear how this can happen
ChatGPT said:

Let me explain why this happens, not just that it happens.
The core of the mystery is:

Why does ls work, but lldb -- ls -la crashes inside glibc (alias_compare)?

This is confusing until you understand what glibc does during startup, what LD_PRELOAD does, and how a custom malloc library affects glibc's internal initialization.

✔️ The key idea

Your malloc library is being used by glibc itself before main() is even reached.

And glibc initialization behaves differently depending on:

the environment variables (LANG, LC_*…)

whether the program is launched normally or via LLDB

what parts of glibc are initialized (locale processing, sorting, alias lookup…)

This leads to glibc running code paths that your normal ls execution never touches.

If any of those code paths require behavior that your malloc implementation does not guarantee (alignment, zeroing, metadata correctness, valid free), then glibc will crash before your program even starts.

✔️ 1. LD_PRELOAD replaces malloc globally, even inside glibc

LD_PRELOAD forces glibc to use your malloc, free, realloc, calloc.

But glibc expects:

16-byte alignment (x86_64 ABI)

valid pointers for qsort parameters

no metadata overwriting

correct calloc() zeroing

correct realloc() resizing

If any of these fail, glibc’s internal structures become corrupted.

This is how you get:

SIGSEGV in alias_compare


Meaning: glibc is dereferencing garbage pointers created by your allocator.

✔️ 2. LLDB changes the environment (this is the smoking gun)

LLDB usually exports a full set of locale variables:

LANG=en_US.UTF-8
LC_CTYPE=en_US.UTF-8
LC_MESSAGES=en_US.UTF-8


Your normal terminal may not.

When glibc sees a non-C locale, it loads:

/usr/share/locale/locale.alias

localization databases

message catalogs

This requires:

reading files

parsing strings

allocating arrays of pointers

sorting them with qsort_r

qsort_r() → calls alias_compare() → crashes, because your malloc has corrupted the array entries.

That’s why the crash happens only when LLDB runs ls.

✔️ 3. Why specifically qsort_r() and alias_compare()?

Because during locale initialization, glibc builds an array of structs:

struct alias_map {
    char *alias;
    char *value;
}


Glibc then calls:

qsort_r(array, n, sizeof(...), alias_compare, <context>);


alias_compare() dereferences:

*(char**)rsi
*(char**)rdi


If your allocator:

returns misaligned pointers

returns pointers into corrupted metadata

overwrote part of the allocation

fails to zero memory

frees too much or merges wrong blocks

…then one of these pointers becomes invalid → segfault.

✔️ 4. Why does plain ls work?

Because plain ls normally runs under the C locale:

LANG=C


Under the C locale:

glibc does not load alias files

does not allocate those arrays

does not qsort anything

so your malloc is used much less before main()

no crash… yet

✔️ 5. How to prove this to yourself

Run:

LANG=C ls -la


→ Works.

Then:

LANG=en_US.UTF-8 ls -la


→ Likely crashes.

Then:

strace -e trace=memory,openat ls


or run inside gdb:

gdb --args ls -la


You will see glibc loading locale files in one case and not the other.

✔️ Summary (the clear answer)
➤ Why does this happen?

Because your custom malloc is being used by glibc during its own initialization.
When LLDB runs ls, it triggers a different locale environment causing glibc to:

allocate strings

build alias tables

run qsort

compare pointers

If your malloc corrupts memory even slightly, glibc crashes in alias_compare().

➤ Why not with plain ls?

Plain ls uses the C locale and does not execute the code paths that rely heavily on memory allocation.

➤ So the root cause?

👉 Your malloc implementation is not ABI-compatible with glibc
(i.e., misalignment, metadata corruption, incorrect free/realloc/calloc)

If you want, show me your block struct and your malloc/free logic, and I will locate the exact bug. This kind of crash is extremely characteristic, and I can usually identify the issue within a few minutes.

You said:
ok show me how to use lldb with this context LANG=C ls -la and settings set target.env-vars LD_PRELOAD=/home/yboudoui/Documents/malloc/libft_malloc.so this way I can see the backtrace if it crash
ChatGPT said:

Here is the exact LLDB command sequence to run ls -la with:

LANG=C

your custom LD_PRELOAD

and arguments -la

You have two correct ways to do it.

✅ Method 1 — set all environment variables via LLDB
lldb -- ls


Then inside LLDB:

(lldb) settings set target.run-args "-la"
(lldb) settings set target.env-vars LANG=C
(lldb) settings set target.env-vars LD_PRELOAD=/home/yboudoui/Documents/malloc/libft_malloc.so
(lldb) run


This runs:

LANG=C LD_PRELOAD=/home/yboudoui/Documents/malloc/libft_malloc.so ls -la


If it crashes, LLDB will stop at the fault and you can do:

(lldb) bt

⚠️ Important note

settings set target.env-vars REPLACES the environment by default.

So if you need both variables, you must specify them together:

(lldb) settings set target.env-vars LANG=C LD_PRELOAD=/home/yboudoui/Documents/malloc/libft_malloc.so


Otherwise the second call overwrites the first.

✅ Method 2 — run LLDB with environment variables already set

This is often simpler:

LANG=C \
LD_PRELOAD=/home/yboudoui/Documents/malloc/libft_malloc.so \
lldb -- ls -la


Inside LLDB:

(lldb) run


This guarantees the environment is exactly what you expect.

✅ How to verify inside LLDB

In LLDB you can check the environment with:

(lldb) env


You should see:

LANG=C
LD_PRELOAD=/home/yboudoui/Documents/malloc/libft_malloc.so

If you want to be extremely explicit
lldb


Then:

(lldb) target create "/usr/bin/ls"
(lldb) settings set target.run-args "-la"
(lldb) settings set target.env-vars "LANG=C" "LC_ALL=C" \
       "LD_PRELOAD=/home/yboudoui/Documents/malloc/libft_malloc.so"
(lldb) run

If it still doesn’t crash

Try this:

LANG=en_US.UTF-8 lldb -- ls -la


If that crashes but LANG=C doesn't, then the bug is triggered by glibc’s locale alias logic — confirming your malloc has an alignment/metadata issue.

If you want, send me the LLDB backtrace and I’ll interpret it.