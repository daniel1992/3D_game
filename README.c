void DrawFrame()
{
	// 紅 X
	glBegin(GL_LINES);
	glColor3ub(255,0,0);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);
	glEnd();
	// 黃 Y
	glBegin(GL_LINES);
	glColor3ub(255,255,0);
	glVertex3f(0,0,0);
	glVertex3f(0,1,0);
	glEnd();
	// 綠 Z
	glBegin(GL_LINES);
	glColor3ub(0,255,0);
	glVertex3f(0,0,0);
	glVertex3f(0,0,1);
	glEnd();

	glColor3ub(255,255,255);
}
