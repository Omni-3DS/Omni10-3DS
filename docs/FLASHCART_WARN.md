# R4 / DSTT warning path (O10-Inst-Booter)

When the user opens **Flashcart (R4 / DSTT)** in Other options:

## Warning screen

```
Please get a magnet and activate sleep mode
then enter in browser http://<ip>:1089
Press A to continue
```

`<ip>` = console LAN IP when available, else `ip` placeholder.

## Color sequence

After **A**:

- Every **2 seconds** one of **5 colors** (notification LED + on-screen):
  1. Red
  2. Green
  3. Blue
  4. Yellow
  5. White
- Order is random per run (stored in `app->colorSeq`).
- User must **order the same 5 colors on the server** page at `http://ip:1089` to confirm the action.

Omni10 itself remains a **Luma FIRM** path; this UI is only the flashcart warning / pairing flow.
