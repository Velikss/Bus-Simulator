#pragma once

#include <pch.hpp>

struct tLine
{
    float fX1;
    float fY1;
    float fX2;
    float fY2;
};

struct tRectangle
{
    glm::vec2 aVertices[4];
};

class cCollisionHelper
{
public:
    // Transform a rectangle with a given transformation matrix
    static tRectangle TransformRectangle(tRectangle tRectangle, glm::mat4 tMatrix);

    // Returns true if two rectangles collide
    static bool Collides(tRectangle tRectangleA, tRectangle tRectangleB);

    // Returns true if the rectangle collides with the line
    static bool CollidesWithLine(tRectangle tRectangle, tLine tLine);

    // Returns true if the two lines intersect
    static bool LinesIntersect(tLine tLineA, tLine tLineB);
};

tRectangle cCollisionHelper::TransformRectangle(tRectangle tRectangleA, glm::mat4 tMatrix)
{
    tRectangle tTransformed = {};

    // Loop over all the vertices in the rectangle and transform them using the matrix
    for (uint uiIndex = 0; uiIndex < 4; uiIndex++)
    {
        glm::vec2 tVertex = tRectangleA.aVertices[uiIndex];
        // The X and Y components of the rectangle represent the X and Z components in the transformation
        glm::vec4 tTransformedVertex = tMatrix * glm::vec4(tVertex.x, 0, tVertex.y, 1);
        tTransformed.aVertices[uiIndex] = glm::vec2(tTransformedVertex.x, tTransformedVertex.z);
    }

    return tTransformed;
}

bool cCollisionHelper::Collides(tRectangle tRectangleA, tRectangle tRectangleB)
{
    // Loop over all vertices in rectangle A
    for (uint uiCurrent = 0; uiCurrent < 4; uiCurrent++)
    {
        // Get the next vertex and wrap back to vertex 0 if this is the last one
        uint uiNext = uiCurrent + 1 == 4 ? 0 : uiCurrent + 1;

        // Create a line from the current to the next vertex
        tLine tLine = {};
        tLine.fX1 = tRectangleA.aVertices[uiCurrent].x;
        tLine.fY1 = tRectangleA.aVertices[uiCurrent].y;
        tLine.fX2 = tRectangleA.aVertices[uiNext].x;
        tLine.fY2 = tRectangleA.aVertices[uiNext].y;

        // If this line collides with rectangle B, we know the two rectangles intersect
        if (CollidesWithLine(tRectangleB, tLine))
        {
            return true;
        }
    }

    return false;
}

bool cCollisionHelper::CollidesWithLine(tRectangle tRectangle, tLine tLineA)
{
    // Loop over all vertices in the rectangle
    for (uint uiCurrent = 0; uiCurrent < 4; uiCurrent++)
    {
        // Get the next vertex and wrap back to vertex 0 if this is the last one
        uint uiNext = uiCurrent + 1 == 4 ? 0 : uiCurrent + 1;

        // Create a line from the current to the next vertex
        tLine tLineB = {};
        tLineB.fX1 = tRectangle.aVertices[uiCurrent].x;
        tLineB.fY1 = tRectangle.aVertices[uiCurrent].y;
        tLineB.fX2 = tRectangle.aVertices[uiNext].x;
        tLineB.fY2 = tRectangle.aVertices[uiNext].y;

        // If the lines intersect we know this rectangle collides with the line
        if (LinesIntersect(tLineA, tLineB))
        {
            return true;
        }
    }

    return false;
}

bool cCollisionHelper::LinesIntersect(tLine tLineA, tLine tLineB)
{
    /*
     * I'm not going to explain this code here, it just checks if two
     * lines intersect. For more info on the theory behind this, check
     * http://paulbourke.net/geometry/pointlineplane/
     */

    float fD1 = (tLineB.fX2 - tLineB.fX1) * (tLineA.fY1 - tLineB.fY1) -
                (tLineB.fY2 - tLineB.fY1) * (tLineA.fX1 - tLineB.fX1);
    float fD2 = (tLineB.fY2 - tLineB.fY1) * (tLineA.fX2 - tLineA.fX1) -
                (tLineB.fX2 - tLineB.fX1) * (tLineA.fY2 - tLineA.fY1);
    float fD = fD1 / fD2;

    float fE1 = (tLineA.fX2 - tLineA.fX1) * (tLineA.fY1 - tLineB.fY1) -
                (tLineA.fY2 - tLineA.fY1) * (tLineA.fX1 - tLineB.fX1);
    float fE2 = (tLineB.fY2 - tLineB.fY1) * (tLineA.fX2 - tLineA.fX1) -
                (tLineB.fX2 - tLineB.fX1) * (tLineA.fY2 - tLineA.fY1);
    float fE = fE1 / fE2;

    return fD >= 0 && fD <= 1 && fE >= 0 && fE <= 1;
}
