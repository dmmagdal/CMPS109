// $Id: shape.cpp,v 1.1 2015-07-16 16:47:51-07 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
#include <string>
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

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
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
            polygon({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   dimension.xpos = width;
   dimension.ypos = height;
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat width): polygon({}) {
   DEBUGF ('c', this);
   dimension.xpos = width;
   dimension.ypos = width;
}

diamond::diamond (GLfloat width, GLfloat height): polygon({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   dimension.xpos = width;
   dimension.ypos = height;
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   auto u_str = reinterpret_cast<const GLubyte*> (textdata.c_str());

}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   const float delta = 2*M_PI/64;
   for (float theta = 0; theta < 2*M_PI; theta += delta){
      float xpos = dimension.xpos * cos(theta) + center.xpos;
      float ypos = dimension.ypos * sin(theta) + center.ypos;
      glVertex2f(xpos, ypos);
   } 
   glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   float xtotal = 0;
   float ytotal = 0;
   for (const auto& i:vertices){
      xtotal += i.xpos;
      ytotal += i.ypos;
   }
   float xavg = xtotal/vertices.size();
   float yavg = ytotal/vertices.size();
   for (const auto& i:vertices){
      float xpos = i.xpos-xavg;
      float ypos = i.ypos-yavg;
      glVertex2f(xpos, ypos);
   } 
   glEnd();
}

void rectangle::draw (const vertex& center, const rgbcolor& color) 
      const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   float xoffset = dimension.xpos/2;
   float yoffset = dimension.ypos/2;
   glVertex2f(center.xpos-xoffset, center.ypos+yoffset);
   glVertex2f(center.xpos+xoffset, center.ypos+yoffset);
   glVertex2f(center.xpos+xoffset, center.ypos-yoffset);
   glVertex2f(center.xpos-xoffset, center.ypos-yoffset);
   glEnd();
}

void square::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   float xoffset = dimension.xpos/2;
   float yoffset = dimension.ypos/2;
   glVertex2f(center.xpos-xoffset, center.ypos+yoffset);
   glVertex2f(center.xpos+xoffset, center.ypos+yoffset);
   glVertex2f(center.xpos+xoffset, center.ypos-yoffset);
   glVertex2f(center.xpos-xoffset, center.ypos-yoffset);
   glEnd();
}

void equilateral::draw (const vertex& center, const rgbcolor& color)
      const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   float offset = dimension.xpos/2;
   glVertex2f(center.xpos-offset, center.ypos-offset);
   glVertex2f(center.xpos+offset, center.ypos-offset);
   glVertex2f(center.xpos, center.ypos+offset);
   glEnd();
}

void diamond::draw (const vertex& center, const rgbcolor& color) 
      const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   float offset = dimension.xpos/2;
   glVertex2f(center.xpos-offset, center.ypos);
   glVertex2f(center.xpos, center.ypos+offset);
   glVertex2f(center.xpos+offset, center.ypos);
   glVertex2f(center.xpos, center.ypos-offset);
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

void rectangle::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

void square::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

void equilateral::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

void diamond::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

