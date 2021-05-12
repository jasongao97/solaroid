/**
 * Processing read photo data from serial communication
 * Built for debug in the Solaroid Project
 */

import processing.serial.*;

Serial myPort;  // Create object from Serial class
int R, G, B, A;

char[] inBuffer = new char[15000];

int count = 0;

void setup() 
{
  size(400, 300);
  myPort = new Serial(this, "/dev/cu.usbserial-AB0KRMN3", 115200);
  myPort.write('A');
}

void draw()
{
  if (count >= 15000) {
    loadPixels();
    
    //for (int x = 0; x < width; x++) {
    //  for (int y = 0; y < height; y++) {
    //    int origin = y * width + x;
    //    int gray = (int) inBuffer[origin];
    //    //if (inBuffer[origin] > 100) {
    //    //  gray = 255;
    //    //}
    //    PxPSetPixel(x, y, gray, gray, gray, 255, pixels, width);
    //  }
    //}
    

    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        int origin = y * width + x;
        int gray = 0;
        //println((origin - (origin % 8)) / 8);
        if (((inBuffer[(origin - (origin % 8)) / 8] >> (7 - origin % 8)) & 0x01) == 1) {
          gray = 255;
        }
        PxPSetPixel(x, y, gray, gray, gray, 255, pixels, width);
      }
    }

    updatePixels();
    count = 0;
  }
}

void serialEvent(Serial myPort) {
  char inByte = (char) myPort.read();

  inBuffer[count] = inByte;

  count++;
}

// our function for getting color components , it requires that you have global variables
// R,G,B   (not elegant but the simples way to go, see the example PxP methods in object for 
// a more elegant solution

void PxPGetPixel(int x, int y, int[] pixelArray, int pixelsWidth) {
  int thisPixel=pixelArray[x+y*pixelsWidth];     // getting the colors as an int from the pixels[]
  A = (thisPixel >> 24) & 0xFF;                  // we need to shift and mask to get each component alone
  R = (thisPixel >> 16) & 0xFF;                  // this is faster than calling red(), green() , blue()
  G = (thisPixel >> 8) & 0xFF;   
  B = thisPixel & 0xFF;
}

//our function for setting color components RGB into the pixels[] , we need to efine the XY of where
// to set the pixel, the RGB values we want and the pixels[] array we want to use and it's width

void PxPSetPixel(int x, int y, int r, int g, int b, int a, int[] pixelArray, int pixelsWidth) {
  a =(a << 24);                       
  r = r << 16;                       // We are packing all 4 composents into one int
  g = g << 8;                        // so we need to shift them to their places
  color argb = a | r | g | b;        // binary "or" operation adds them all into one int
  pixelArray[x+y*pixelsWidth]= argb;    // finaly we set the int with te colors into the pixels[]
}
