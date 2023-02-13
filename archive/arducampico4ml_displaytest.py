# This is a circuitpython test script for the lcd display on the arducam pico4ml
# The board has a RP2040 on it. The firmware used is circuitpython 7.3.3
# It is NOT working at the moment. No clear information found about the display
# used on this kit. According to the library provide by Arducam. The display could
# be a st7735 compitable display. No luck yet. According to the schematic of 
# arducam pico4ml from sparkfun. The display could be UCTRONICS-096-LCD. The sche-
# matic link is https://www.arducam.com/downloads/Arducam-Pico4ML-board-Shematics-UC-798_SCH.pdf
import board
import busio
import displayio
import time
from adafruit_st7735r import ST7735R

#Pin connection:
#| LCD  | RST   | DC    | CS     | CLK    | MOSI   |
#| ---- | ----- | ----- | ------ | ------ | ------ |
#| PICO | GPIO7 | GPIO9 | GPIO13 | GPIO10 | GPIO11 |

displayio.release_displays()

spi = busio.SPI(board.GP10, MOSI=board.GP11)
lcd_cs = board.GP13
lcd_dc = board.GP9

display_bus = displayio.FourWire(spi, command=lcd_dc, chip_select=lcd_cs, reset=board.GP7)

display = ST7735R(display_bus, width=80, height=160, colstart=24, rowstart=0, bgr=True)

# Make the display context
splash = displayio.Group()
display.show(splash)
time.sleep(1)
color_bitmap = displayio.Bitmap(80, 160, 2)
color_palette = displayio.Palette(3)
color_palette[0] = 0x00FFFF

bg_sprite = displayio.TileGrid(color_bitmap, pixel_shader=color_palette, x=0, y=0)
splash.append(bg_sprite)
time.sleep(1)
# Draw a smaller inner rectangle
inner_bitmap = displayio.Bitmap(150, 118, 1)
inner_palette = displayio.Palette(3)
inner_palette[0] = 0xAA0088  # Purple
inner_sprite = displayio.TileGrid(inner_bitmap, pixel_shader=inner_palette, x=5, y=5)
splash.append(inner_sprite)

while True:
    pass
