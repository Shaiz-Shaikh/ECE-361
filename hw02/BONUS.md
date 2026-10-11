# Bonus: how deep can a recursive maximum go?

## My computer

- MacBook Air, Apple M1
- Compiler: Apple clang version 21.0.0 (clang-2100.3.34.2), run as `gcc`
- Stack size limit: 8176 KB (`ulimit -s`)
- Test program: `tests/depth.c`. The array lives on the heap (`malloc`), so only the recursion itself uses the stack.

## Measurements

The largest n that does not crash, found by doubling n until a crash and then bisecting until the gap was under 10 %:

| Version          | -O0                                   | -O2                                   |
|------------------|---------------------------------------|---------------------------------------|
| `stats_max`      | survives 170,000, crashes at 180,000  | survives 260,000, crashes at 280,000  |
| `stats_max_tail` | survives 170,000, crashes at 180,000  | survives (tested up to 40,960,000)    |

## Explanation

**Why does stats_max_tail survive at -O2?** In `max_tail_helper` the recursive call is the last thing the function does, and its result is returned as it is. Nothing in the current call is needed afterwards, so the compiler can replace the call with a jump back to the top of the function. The whole chain then reuses one stack frame, and the stack does not grow with n.

**Why does it not survive at -O0?** At -O0 the compiler does no optimization, so it translates the call literally. Every call gets its own frame, and the stack grows by one frame per element. The tail version crashes at the same size as the normal version (170,000 to 180,000), so writing a tail call does not help unless the compiler uses it. C does not promise that it will.

**Why does -O2 not save stats_max?** After the recursive call returns, `stats_max` still has work to do: it compares `a[0]` with the result. The call is not the last thing it does, so each call has to wait, and the compiler cannot turn it into a jump. -O2 only made each frame smaller, so it survives longer (260,000 against 170,000), but the stack still grows with n. With an 8176 KB stack, that is roughly 48 bytes per call at -O0 and roughly 32 bytes per call at -O2. These are estimates from the midpoints of my ranges.

**What does this mean for a recursive function on a binary search tree?** The depth of the recursion equals the height of the tree. In a balanced tree the height is about log2(n), which is tiny, so recursion is safe. But a tree built from sorted input degenerates into a linked list of height n, and then a recursive insert or find can overflow the stack just like `stats_max`. Many tree functions, such as an in-order walk, also do work after the recursive call, so they are not tail calls and -O2 cannot rescue them. In my weather station, temperatures that rise steadily would produce increasing keys, so I should check the height of my tree and not assume it is balanced.
