/*
	Filename: IndieBox2DRender.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: A class which manages rendering (in debug mode) of debug info of Box2D physics library
	Comments: It uses main application renderer, that is, mpILib.
	Attribution: Based in class made by user "ehudros" from IndieLib forums (http://www.mpILib.com/forum/viewtopic.php?t=240&sid=a976f5258c860f6635957434187fc857) GOOD WORK!!!
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "IndieBox2DRender.h"
#include "IndieLibManager.h"

void IndieBox2DRender::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
   if(!mpILib) return;
   
   //mpILib->Gfx_BeginScene();
   for (int32 i = 0, j = 1; i < vertexCount; ++i, j++)
   {
      j = (j>=vertexCount)?0:j;
	  mpILib->Render->BlitLine(static_cast<int>(mScale * vertices[i].x),
							   static_cast<int>(mOffsetY - mScale * vertices[i].y),
							   static_cast<int>(mScale * vertices[j].x),
							   static_cast<int>(mOffsetY - mScale * vertices[j].y),
							   255,
							   static_cast<byte>(color.r*255),
							   static_cast<byte>(color.g*255),
							   static_cast<byte>(color.b*255)
							   );
   }
   //mpILib->Gfx_EndScene();
}

void IndieBox2DRender::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
   if(!mpILib) return;
   
   //mpILib->Gfx_BeginScene();
   for (int32 i = 0, j = 1; i < vertexCount; ++i, j++)
   {
      j = (j>=vertexCount)?0:j;
	  mpILib->Render->BlitLine(static_cast<int>(mScale * vertices[i].x),
							   static_cast<int>(mOffsetY - mScale * vertices[i].y),
							   static_cast<int>(mScale * vertices[j].x),
							   static_cast<int>(mOffsetY - mScale * vertices[j].y), 
							   200,
							   static_cast<byte>(color.r*255),
							   static_cast<byte>(color.g*255),
							   static_cast<byte>(color.b*255)
							   );
   }
   //mpILib->Gfx_EndScene();
}

void IndieBox2DRender::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
   if(!mpILib) return;
 
   int NUMPOINTS = 24+(int)radius/20;

   std::vector<b2Vec2> vertices;
   int i;//,j;
   float X;
   float Y;
   float Theta;
   float WedgeAngle;   //Size of angle between two points on the circle (single wedge)

   //Precompute WedgeAngle
   WedgeAngle = static_cast<float>(((2*PI) / NUMPOINTS));

   //Set up vertices for a circle
   //Used <= in the for statement to ensure last point meets first point (closed circle)
   for(i=0; i<=NUMPOINTS; i++)
   {
      //Calculate theta for this vertex
      Theta = i * WedgeAngle;
     
      //Compute X and Y locations
      X = static_cast<float>((center.x + radius * cos(Theta)));
      Y = static_cast<float>((center.y - radius * sin(Theta)));

      vertices.push_back(b2Vec2(X, Y));
   }

   //mpILib->Gfx_BeginScene();
   for (int32 k = 0, j = 1; k < (signed)vertices.size(); ++k, ++j)
   {
      j = (j>=(signed)vertices.size())?0:j;
	  mpILib->Render->BlitLine(static_cast<int>(mScale * vertices[k].x),
							   static_cast<int>(mOffsetY - mScale * vertices[k].y),
							   static_cast<int>(mScale * vertices[j].x),
							   static_cast<int>(mOffsetY - mScale * vertices[j].y),
							   255,
							   static_cast<byte>(color.r*255),
							   static_cast<byte>(color.g*255),
							   static_cast<byte>(color.b*255)
							   );
   }
   //mpILib->Gfx_EndScene();
}

void IndieBox2DRender::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2&, const b2Color& color)
{
   if(!mpILib) return;

   int NUMPOINTS = 24+(int)radius/20;

   std::vector<b2Vec2> vertices;

   int i;//,j;
   float X;
   float Y;
   float Theta;
   float WedgeAngle;   //Size of angle between two points on the circle (single wedge)

   //Precompute WedgeAngle
   WedgeAngle = static_cast<float>(((2*PI) / NUMPOINTS));

   //Set up vertices for a circle
   //Used <= in the for statement to ensure last point meets first point (closed circle)
   for(i=0; i<=NUMPOINTS; i++)
   {
      //Calculate theta for this vertex
      Theta = i * WedgeAngle;
     
      //Compute X and Y locations
      X = static_cast<float>((center.x + radius * cos(Theta)));
      Y = static_cast<float>((center.y - radius * sin(Theta)));

      vertices.push_back(b2Vec2(X, Y));//Circle[i] = mpILibVector(X, Y);
   }

   //mpILib->Gfx_BeginScene();
   
   for (int32 k = 0, j = 1; k < (signed)vertices.size(); ++k, ++j)
   {
      j = (j>=(signed)vertices.size())?0:j;
	  mpILib->Render->BlitLine(static_cast<int>(mScale * vertices[k].x), 
							   static_cast<int>(mOffsetY - mScale * vertices[k].y),
							   static_cast<int>(mScale * vertices[j].x),
							   static_cast<int>(mOffsetY - mScale * vertices[j].y),
							   200,
							   static_cast<byte>(color.r*255),
							   static_cast<byte>(color.g*255),
							   static_cast<byte>(color.b*255)
							   );
   }
   //mpILib->Gfx_EndScene();
}

void IndieBox2DRender::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
   if(!mpILib) return;

   //mpILib->Gfx_BeginScene();
   mpILib->Render->BlitLine(static_cast<int>(mScale * p1.x), 
							static_cast<int>(mOffsetY - mScale * p1.y),
						    static_cast<int>(mScale * p2.x),
						    static_cast<int>(mOffsetY- mScale * p2.y), 
							255,
							static_cast<byte>(color.r*255),
							static_cast<byte>(color.g*255),
							static_cast<byte>(color.b*255)
							);
   //mpILib->Gfx_EndScene();
}

void IndieBox2DRender::DrawXForm(const b2XForm& xf)
{
   if(!mpILib) return;

   b2Vec2 p1 = xf.position, p2;
   const float32 k_axisScale = 0.4f;
   //mpILib->Gfx_BeginScene();
   
   p2 = p1 + k_axisScale * xf.R.col1;
   mpILib->Render->BlitLine(static_cast<int>(mScale * p1.x), 
							static_cast<int>(mOffsetY - mScale * p1.y),
							static_cast<int>(mScale * p2.x),
							static_cast<int>(mOffsetY - mScale * p2.y), 
							255,
							255,
							0,
							0);
   p2 = p1 + k_axisScale * xf.R.col2;
   mpILib->Render->BlitLine(static_cast<int>(mScale * p1.x), 
						    static_cast<int>(mOffsetY - mScale * p1.y),
							static_cast<int>(mScale * p2.x), 
							static_cast<int>(mOffsetY - mScale * p2.y), 
							255,
							0,
							255,
							0);
   //mpILib->Gfx_EndScene();
} 