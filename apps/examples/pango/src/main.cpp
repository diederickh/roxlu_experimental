#include <pango/TextSurface.h>
#include <iostream>
#include <stdio.h>

int main() {
  
  {
    TextSurface ts;
    ts.setup("Arial 10", 700,700);
    ts.setTextWidth(200);
    ts.wrapWord();
    ts.alignRight(); 
    ts.setMarkup("<span font='Courier 12' fgcolor='#000000'>And a bit in courier</span><span fgcolor='#00FFFF' underline='double'> and some more</span><span font='Helvetica Neue 15' bgcolor='#CC00CC' fgcolor='#FFFFFF'> and even more.</span>");
    ts.render();
    ts.saveAsPNG("pango1.png");
  }

  {
    TextSurface ts;
    ts.setup("Arial 50", 700, 128);
    ts.fill(1.0, 1.0, 0.0, 0.5);
    ts.setText("Lorem ipsum dolor sit");
    ts.render();
    ts.saveAsPNG("pango2.png");
  }

  {
    TextSurface ts;
    ts.setup("sans-serif 50", 700, 128);
    ts.fill(1.0, 1.0, 0.0, 0.0);
    ts.setText("Lorem ipsum dolor sit");
    ts.render();
    ts.saveAsPNG("pango3.png");
  }

  {
    TextSurface ts;
    ts.setup("Arial 50", 700, 128);
    ts.fill(1.0, 0.0, 0.0, 0.0);
    ts.setText("Lorem ipsum dolor sit", 1.0f, 1.0f, 1.0f);
    ts.render();
    ts.saveAsPNG("pango4.png");
  }
  return 0;
}
