#define DEBUG 0

#if DEBUG
	#define OPENGL_DEBUG        1
	#define FULLSCREEN          0
	#define STRETCH				1 
#else
	#define OPENGL_DEBUG        0
	#define FULLSCREEN          1
	#define STRETCH				1 
#endif
#define USE_AUDIO    1
#define NO_UNIFORMS  1

#include "definitions.h"
#if OPENGL_DEBUG
	#include "debug.h"
#endif
#include "glext.h"
#include "shaders/fragment.inl"

#pragma data_seg(".pids")
static int pidMain;
static int pidPost;

GLuint base;

void writeText(char* text, float x, float y){
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -1.0f);
	glRasterPos2d(x, y);

	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

#pragma code_seg(".main")
void entrypoint(void) {
	#if FULLSCREEN
		ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN);
		ShowCursor(0);
		const HDC hDC = GetDC(CreateWindow((LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0));
	#else
		#if STRETCH
			const HDC hDC = GetDC(CreateWindow((LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE , 0, 0, XRES, YRES, 0, 0, 0, 0));
		#else		
			const HDC hDC = GetDC(CreateWindow((LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0));
		#endif
	#endif  

	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));

	HFONT font;
	base = glGenLists(96);
	font = CreateFont(-70, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Georgia");
	SelectObject(hDC, font);
	wglUseFontBitmaps(hDC, 32, 96, base);

	const auto oglUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	const auto program = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &fragment);

	#if USE_AUDIO
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, lpSoundBuffer, 0, 0);
		waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL);
		waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
		waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	#endif
		 
	glBindTexture(GL_TEXTURE_2D, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	float time = 0.;

	do {

		time = MMTime.u.sample / (SAMPLE_RATE * 60. / BPM);

		#if USE_AUDIO
			waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));

			#if NO_UNIFORMS
				glTexCoord3d(time, 0, 0);
			#else
				((PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i"))(0, MMTime.u.sample);
			#endif
		#endif

		oglUseProgram(program);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, XRES, YRES, 0);
		glRects(-1, -1, 1, 1); 



		oglUseProgram(0);

		if (time > 244.) {
			glColor3f(.5, .5, .8);
			writeText("code", -0.9, -0.75);
		}
		if (time > 245.5) {
			glColor3f(1., 1., 1.);
			writeText("anticore", -0.69, -0.75);
		}
		if (time > 247.) {
			glColor3f(.8, .5, .5);
			writeText("music", -0.3, -0.75);
		}
		if (time > 248.5) {

			glColor3f(1., 1., 1.);
			writeText("jeenio", -0.05, -0.75);
		}
		if (time > 252.) {
			glColor3f(.8, .8, .8);
			writeText("scenePT all stars 2021", -0.9, -0.9);
		}

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);


		glColor3f(1., 1., 1.);

		if (time < 223.) {
			float t = -0.8;
			writeText("alien", -0.9, t);
			writeText("crmx", -0.4, t);
			writeText("jae686", 0.1, t);
			writeText("lordcoxis", 0.6, t);
			t = -0.6;
			writeText("ps", -0.9, t);
			writeText("dextrose", -0.55, t);
			writeText("gr9yfox", 0.0, t);
			writeText("rup", 0.4, t);
			writeText("mirage", 0.65, t);
			t = -0.4;
			writeText("xernobyl", -0.95, t);
			writeText("EviL", -0.45, t);
			writeText("PauloFalcao", -0.05, t);
			writeText("zeroshift", 0.65, t);
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT);
			writeText("mafamood", -0.1, -0.62);
		}

		SwapBuffers(hDC);

	} while(!GetAsyncKeyState(VK_ESCAPE)
		#if USE_AUDIO
			&& MMTime.u.sample < MAX_SAMPLES
		#endif
	);

	ExitProcess(0);
}
