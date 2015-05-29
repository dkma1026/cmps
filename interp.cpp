// Diana Ma
// dkma@ucsc.edu
// Asg 4: OOP Inheritance
// interp.cpp

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

map<string,interpreter::interpreterfn> interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border" , &interpreter::do_border },
   {"moveby" , &interpreter::do_moveby },
};

map<string,interpreter::factoryfn> interpreter::factory_map {
   {"text"           , &interpreter::make_text           },
   {"ellipse"        , &interpreter::make_ellipse        },
   {"circle"         , &interpreter::make_circle         },
   {"polygon"        , &interpreter::make_polygon        },
   {"rectangle"      , &interpreter::make_rectangle      },
   {"square"         , &interpreter::make_square         },
   {"diamond"        , &interpreter::make_diamond        },
   {"triangle"       , &interpreter::make_triangle       },
   {"right_triangle" , &interpreter::make_right_triangle },
   {"isosceles"      , &interpreter::make_isosceles      },
   {"equilateral"    , &interpreter::make_equilateral    }   
 //{"border"         , &interpreter::make_border         },
};

// 
// Constructor for the interpreter class.
//
interpreter::shape_map interpreter::objmap;

// Destructor for the interpreter class.
//

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
    // First word contains command name
   // Either define, draw, border, or moveby
   string command = *begin;
   DEBUGF('i', command);
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   for (auto i = begin; i != params.cend(); ++i) {
      cout << *i << " ";
   } cout << endl;
   func (++begin, params.cend());
}


void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   DEBUGF ('t', name);
   objmap.insert ({name, make_shape (++begin, end)});
   //for (; begin != end; ++begin) //del
      //cout << *begin << "\t"; //del
   //cout << endl; //del
}

void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   rgbcolor color {begin[0]};
   object new_obj(itor->second, where, color);
   window::push_back(new_obj);
   //itor->second->draw (where, color);
}

void interpreter::do_border (param begin, param end) {
   if ((end - begin) != 2) throw runtime_error("syntax error");
   rgbcolor color{*begin++};
   float thickness = stod(*begin);
   window::set_border(color,thickness);
}

void interpreter::do_moveby (param begin, param end) {
    //window::set_pixel(pix);
   if ((end - begin) != 1) throw runtime_error("syntax error");
   window::set_moveby(float(stod(*begin)));
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

// make_text
shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string font = *begin;
   string data{};
   // concat list of words into space delimited string
   for (++begin; begin != end; ++begin) {
      DEBUGF ('t', *begin);
      data += *begin + " ";
      //cout << str << endl;
   }
   return make_shared<text> (font, data);
}

// make_ellipse
shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //
   // Pop the front two strings from the command list.  
   // Try to convert them to a double and set them as height and width respectively.
   // If an error is caught, throw a syntax error.
   //
   if ((end - begin) != 2) throw runtime_error("syntax error: in ellipse");
   GLfloat width;
   GLfloat height;
   width = stod(*begin++);
   height = stod(*begin);
   return make_shared<ellipse> (width, height);
}
// make_circle
shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //
   // Pop the diameter string from the command list.  
   // Try to convert the string to a double.  
   // If any errors are caught, throw a syntax error.
   //
   if ((end - begin) != 1) throw runtime_error("syntax error: in circle");
   return make_shared<circle> (GLfloat(stod(*begin)));
}

// make_polygon
shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //
   if (((end - begin) % 2) != 0) {
      throw runtime_error ("syntax error: vertex list must have the same length");
   }
   float x_avg{};
   float y_avg{};
   int v_count{0};
   vector<vertex> v_list;

   for (auto i = begin; i != end; i++) {
      GLfloat xpos = stod(*i++);
      GLfloat ypos = stod(*i);
      x_avg += xpos;
      y_avg += ypos;
      vertex v{xpos, ypos};
      v_count++;
      v_list.push_back(v);
   }
   if (v_count == 0) 
      throw runtime_error ("syntax error: vertex list must be not empty");
   x_avg /= v_count;
   y_avg /= v_count;
   DEBUGF ('p', "avg: (" << x_avg << "," << y_avg << ")");
   for (auto v = v_list.begin(); v != v_list.end(); ++v) {
      v->xpos -= x_avg;
      v->ypos -= y_avg;
   }
   std::sort (v_list.begin(), v_list.end(),
      [](const vertex &a, const vertex &b){
      float degree_a = atan2(a.ypos, a.xpos) * 180 / M_PI;
      float degree_b = atan2(b.ypos, b.xpos) * 180 / M_PI;
      return (degree_a < degree_b);
   });
   return make_shared<polygon> (vertex_list(v_list));
}

// make_rectangle
shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //
   // Pop the first two string arguments from the command list.  
   // Try to convert them to double.  
   // If any errors are caught, throw a syntax error.
   //
   if ((end - begin) != 2) {
      throw runtime_error("syntax error: in rectangle");
   }
   return make_shared<rectangle> (width, height);
   GLfloat width;
   GLfloat height;
   width = stod(*begin++);
   height = stod(*begin);
}

// make_square
shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   // Pop the width string from the command list.  
   // Try to convert the string to a double.  
   // If any errors are caught, throw a syntax error.
   //
   if ((end - begin) != 1) throw runtime_error("syntax error: in square");
   return make_shared<square> (GLfloat(stod(*begin)));
}

// make_diamond
shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //
   if ((end - begin) != 2) {
      throw runtime_error("syntax error: in diamond");
   }
   return make_shared<diamond> (width, height);
    GLfloat width;
   GLfloat height;
   width = stod(*begin++);
   height = stod(*begin);
}

// make_triangle 
shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   //
   if ((end - begin) != 6) {
      throw runtime_error("syntax error: in triangle");
   }
   return (make_shared<triangle>(v0, v1,v2));
    GLfloat x0,y0,x1,y1,x2,y2;
   x0 = stod(*begin++);
   y0 = stod(*begin++);
   vertex v0{x0,y0};
   x1 = stod(*begin++);
   y1 = stod(*begin++);
   vertex v1{x1,y1};
   x2 = stod(*begin++);
   y2 = stod(*begin);
   vertex v2{x2,y2};
}

// make_right_triangle
shape_ptr interpreter::make_right_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if ((end - begin) != 2) {
      throw runtime_error("syntax error: in right_triangle");
   }
   return make_shared<right_triangle> (width, height);
   GLfloat width;
   GLfloat height;
   width = stod(*begin++);
   height = stod(*begin);
}
// make_isosceles
shape_ptr interpreter::make_isosceles (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if ((end - begin) != 2) { 
      throw runtime_error("syntax error: in isosceles");
   }
   return make_shared<isosceles> (width, height);
   GLfloat width;
   GLfloat height;
   width = stod(*begin++);
   height = stod(*begin);
}
// make_equilateral
shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if ((end - begin) != 1) {
      throw runtime_error("syntax error: in equilateral");
   }
   return make_shared<equilateral> (width);
   GLfloat width;
   width = stod(*begin++);
}