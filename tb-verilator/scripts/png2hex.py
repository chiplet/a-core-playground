# SPDX-License-Identifier: Apache-2.0

import argparse
from PIL import Image

def main():
    parser = argparse.ArgumentParser()
    parser.description = "Convert a PNG image into a hex file for RTL memory initialization."
    parser.add_argument("png", help="input")
    args = parser.parse_args()

    im = Image.open(args.png)
    px = im.load()
    for y in range(im.height):
        for x in range(im.width):
            (r,g,b,a) = px[x,y]
            print("{:02x}{:02x}{:02x} ".format(r, g, b), end="")
        print()

if __name__=='__main__':
    main()
