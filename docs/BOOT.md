# Booting Omni10.firm

## Chainload (Luma3DS)

1. Download **Omni10.firm** from Actions artifact `Omni10-firm`
2. Copy to `sd:/luma/payloads/Omni10.firm`
3. Hold **START** on boot → select Omni10

## What you should see

If the payload runs (same style as 3DSFirm):

- Mostly **black** top screen
- **Blue** bar (move with D-Pad Left/Right)
- Small **red** and **green** corners

There is **no** menu and **no** return to Home Menu yet — that looks like a “freeze” but is the infinite main loop.

## Power off

Hold power button; there is no clean exit in this minimal firm.

## Still pure black / hard lock

1. Confirm you use the **Actions-built** firm, not an old placeholder
2. Compare with your working `y_firm_darkfox.firm` from 3DSFirm on the same console
3. If 3DSFirm works and Omni10 does not, open an issue with both firm sizes and Luma version
