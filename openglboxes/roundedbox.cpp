#include "roundedbox.h"

//============================================================================//
//                                GLRoundedBox                                //
//============================================================================//

float lerp(float a, float b, float t)
{
    return a * (1.0f - t) + b * t;
}

GLRoundedBox::GLRoundedBox(float r, float scale, int n)
    : GLTriangleMesh<P3T2N3Vertex, unsigned short>((n+2)*(n+3)*4, (n+1)*(n+1)*24+36+72*(n+1))
{
    int vidx = 0, iidx = 0;
    int vertexCountPerCorner = (n + 2) * (n + 3) / 2;

	//VAO
    P3T2N3Vertex *vp = m_vb.lock();
    //VBO
	unsigned short *ip = m_ib.lock();

    if (!vp || !ip) {
        qWarning("GLRoundedBox::GLRoundedBox: Failed to lock vertex buffer and/or index buffer.");
        m_ib.unlock();
        m_vb.unlock();
        return;
    }

    for (int corner = 0; corner < 8; ++corner) {
        QVector3D centre(corner & 1 ? 1.0f : -1.0f,
                corner & 2 ? 1.0f : -1.0f,
                corner & 4 ? 1.0f : -1.0f);
        int winding = (corner & 1) ^ ((corner >> 1) & 1) ^ (corner >> 2);
        int offsX = ((corner ^ 1) - corner) * vertexCountPerCorner;
        int offsY = ((corner ^ 2) - corner) * vertexCountPerCorner;
        int offsZ = ((corner ^ 4) - corner) * vertexCountPerCorner;

        // Face polygons
        if (winding) {
            ip[iidx++] = vidx;
            ip[iidx++] = vidx + offsX;
            ip[iidx++] = vidx + offsY;

            ip[iidx++] = vidx + vertexCountPerCorner - n - 2;
            ip[iidx++] = vidx + vertexCountPerCorner - n - 2 + offsY;
            ip[iidx++] = vidx + vertexCountPerCorner - n - 2 + offsZ;

            ip[iidx++] = vidx + vertexCountPerCorner - 1;
            ip[iidx++] = vidx + vertexCountPerCorner - 1 + offsZ;
            ip[iidx++] = vidx + vertexCountPerCorner - 1 + offsX;
        }

        for (int i = 0; i < n + 2; ++i) {

            // Edge polygons
            if (winding && i < n + 1) {
                ip[iidx++] = vidx + i + 1;
                ip[iidx++] = vidx;
                ip[iidx++] = vidx + offsY + i + 1;
                ip[iidx++] = vidx + offsY;
                ip[iidx++] = vidx + offsY + i + 1;
                ip[iidx++] = vidx;

                ip[iidx++] = vidx + i;
                ip[iidx++] = vidx + 2 * i + 2;
                ip[iidx++] = vidx + i + offsX;
                ip[iidx++] = vidx + 2 * i + offsX + 2;
                ip[iidx++] = vidx + i + offsX;
                ip[iidx++] = vidx + 2 * i + 2;

                ip[iidx++] = (corner + 1) * vertexCountPerCorner - 1 - i;
                ip[iidx++] = (corner + 1) * vertexCountPerCorner - 2 - i;
                ip[iidx++] = (corner + 1) * vertexCountPerCorner - 1 - i + offsZ;
                ip[iidx++] = (corner + 1) * vertexCountPerCorner - 2 - i + offsZ;
                ip[iidx++] = (corner + 1) * vertexCountPerCorner - 1 - i + offsZ;
                ip[iidx++] = (corner + 1) * vertexCountPerCorner - 2 - i;
            }

            for (int j = 0; j <= i; ++j) {
                QVector3D normal = QVector3D(i - j, j, n + 1 - i).normalized();
                QVector3D offset(0.5f - r, 0.5f - r, 0.5f - r);
                QVector3D pos = centre * (offset + r * normal);

                vp[vidx].position = scale * pos;
                vp[vidx].normal = centre * normal;
                vp[vidx].texCoord = scale * pos;

                // Corner polygons
                if (i < n + 1) {
                    ip[iidx++] = vidx;
                    ip[iidx++] = vidx + i + 2 - winding;
                    ip[iidx++] = vidx + i + 1 + winding;
                }
                if (i < n) {
                    ip[iidx++] = vidx + i + 1 + winding;
                    ip[iidx++] = vidx + i + 2 - winding;
                    ip[iidx++] = vidx + 2 * i + 4;
                }

                ++vidx;
            }
        }

    }

    m_ib.unlock();
    m_vb.unlock();
}

