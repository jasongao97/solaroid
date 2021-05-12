/**
 * Floyd Steinberg Dithering
 * Black & White
 * For testing result on an e-paper screen
 * 
 * Arranged from Daniel Shiffman's orginal code
 */

import processing.video.*;

Capture video;

void setup() {
  size(640, 480);
  video = new Capture(this, width, height, "FaceTime HD Camera (Built-in)");
  video.start();
}

int index(int x, int y) {
  return x + y * video.width;
}

void draw() {
  if (video.available()) video.read();
  video.filter(GRAY);

  video.loadPixels();
  for (int y = 0; y < video.height-1; y++) {
    for (int x = 1; x < video.width-1; x++) {
      color pix = video.pixels[index(x, y)];
      float oldR = red(pix);
      int factor = 1;
      int newR = round(factor * oldR / 255) * (255 / factor);
      video.pixels[index(x, y)] = color(newR, newR, newR);

      float errR = oldR - newR;

      int index = index(x+1, y);
      color c = video.pixels[index];
      float r = red(c);
      r = r + errR * 7/16.0;
      video.pixels[index] = color(r);

      index = index(x-1, y+1);
      c = video.pixels[index];
      r = red(c);
      r = r + errR * 3/16.0;
      video.pixels[index] = color(r);

      index = index(x, y+1);
      c = video.pixels[index];
      r = red(c);
      r = r + errR * 5/16.0;
      video.pixels[index] = color(r);


      index = index(x+1, y+1);
      c = video.pixels[index];
      r = red(c);
      r = r + errR * 1/16.0;
      video.pixels[index] = color(r);
    }
  }

  // for (int y = 0; y < video.height; y++) {
  //   for (int x = 0; x < video.width; x++) {
  //     int p = video.pixels[x];
  //     print(red(p));
  //   }
  //   println();
  // }
  video.updatePixels();
  image(video, 0, 0);
}
