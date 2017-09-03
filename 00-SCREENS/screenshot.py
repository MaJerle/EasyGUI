import serial
import Image
import random


"""
By sending "b" character to serial you are able to read current screen from device if firmware is correct.
"""

PORT = "COM3"
BAUD = 115200*4
IMG_WIDTH = 800
IMG_HEIGHT = 480
DATA_SIZE = IMG_WIDTH * IMG_HEIGHT * 4

s = serial.Serial(PORT, BAUD)
s.timeout = 0.5

img = Image.new("RGBA", (IMG_WIDTH, IMG_HEIGHT), "white")
img.load()

pixels = list(img.getdata())

s.write(b"b");

buff = ""
while len(buff) < DATA_SIZE:
	buff += s.read(1024)
	print "R ", str(len(buff))


arr = bytearray(buff)

newData = []
for i in range(len(pixels)):
	newData.append((arr[i * 4 + 2], arr[i * 4 + 1], arr[i * 4 + 0], arr[i * 4 + 3]))

img.putdata(newData)
img.save("screenshot.png")
