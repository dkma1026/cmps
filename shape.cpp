// Diana Ma
// dkma@ucsc.edu
// Asg 4: OOP Inheritance
// shape.cpp

#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (const string& font_string, const string& textdata):
      font_string(font_string), textdata(textdata) {
   DEBUGF ('c', this);
   glut_bitmap_font = fontcode[font_string];
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
      polygon({{width/2, -height/2},
               {width/2, height/2},
               {-width/2, height/2},
               {-width/2, -height/2}}){
   DEBUGF ('c', this << "(" << w << "," << h << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}


diamond::diamond (GLfloat width, GLfloat height):
      polygon({{width/2, 0},
               {0, height/2},
               {-width/2, 0},
               {0, -height/2}}){
   DEBUGF ('c', this << "(" << w << "," << h << ")");
}

triangle::triangle(const vertex v0, const vertex v1, const vertex v2):
      polygon({v0,v1,v2}) {
   DEBUGF ('c', this);
}

right_triangle::right_triangle(GLfloat width, GLfloat height):
      triangle({width,0},
               {0,height},
               {0,0}) {
   DEBUGF ('c', this);
}
isosceles::isosceles(GLfloat width, GLfloat height):
      triangle({width/2,0},
               {0,height},
               {-width/2,0}) {
   DEBUGF ('c', this);
}
equilateral::equilateral(GLfloat width):
      isosceles(width, width) {
   DEBUGF ('c', this);
}


void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glColor3ubv(color.ubvec);
   glRasterPos2f(center.xpos, center.ypos);
   for (auto ch:textdata) glutBitmapCharacter(glut_bitmap_font, ch);
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
    // dimension has the width and height GLfloats
   // center has the position GLfloats
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = dimension.xpos * cos (theta) + center.xpos;
      float ypos = dimension.ypos * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();   
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   //cout << "av: " << aver_ver.xpos << " , " << aver_ver.ypos << endl;
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   for (auto v : vertices) {
      float x_draw = v.xpos + center.xpos;
      float y_draw = v.ypos + center.ypos;
      DEBUGF ('d', "v(" << x_draw <<"," << y_draw <<")");
      glVertex2f(x_draw, y_draw);
   }
   glEnd();
}


void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}