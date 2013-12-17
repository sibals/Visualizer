#include <iostream>
#include <assert.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "SoundManager.h"
#include "Mars.h"
#include "EndlessSpline.h"

using namespace std;
using namespace glm;

Window window;
GLuint whiteTexHandle;
SoundManager audio;
EndlessSpline ribbon;
EndlessSpline ribbon2;
Mars hiResSphere;

void CloseFunc()
{
        window.window_handle = -1;
		window.sphere.TakeDown();
		hiResSphere.TakeDown();
        window.background.TakeDown();
        window.mars.TakeDown();
        window.rendertexture.TakeDown();
		ribbon.TakeDown();
		ribbon2.TakeDown();	
}

void ReshapeFunc(int w, int h)
{
        if (h > 0)
        {
                window.size = ivec2(w, h);
                window.window_aspect = float(w) / float(h);
        }
}

void KeyboardFunc(unsigned char c, int x, int y)
{
        float current_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;

        switch (c)
        {
        case 'Z':
        case 'z':
                break;
        case 'Q':
        case 'q':
                window.rendertexture.StepPostEffect();
                break;
        case 'W':
        case 'w':
                window.wireframe = !window.wireframe;
                break;
        case 'P':
        case 'p':
                if (window.paused == true)
                {
                        // Will be leaving paused state
						audio.Pause(false);
                        window.total_time_paused += (current_time - window.time_last_pause_began);
                }
                else
                {
                        // Will be entering paused state
						audio.Pause(true);
                        window.time_last_pause_began = current_time;
                }
                window.paused = !window.paused;
                break;
                
        case 'X':
        case 'x':
				if (window.debugTextureID == 1) {
					window.debugTextureID = 2;
				} else {
					window.debugTextureID = 1;
				}
				break;
        case 27:
                glutLeaveMainLoop();
                return;
        }
}

void SpecialFunc(int c, int x, int y)
{
        switch (c)
        {
        case GLUT_KEY_UP:
                if(window.camera.up_down < 2.5f)
                        window.camera.up_down += 0.01f;
                break;

        case GLUT_KEY_DOWN:
                if(window.camera.up_down > 0.0f)
                        window.camera.up_down -= 0.01f;
                break;

        case GLUT_KEY_F1:
                window.mode++;
                if(window.mode > 4)        //change to add more modes
                        window.mode = 0;
                break;

        case GLUT_KEY_LEFT:
                window.camera.x_offset -= 1.0f;
                break;

        case GLUT_KEY_RIGHT:
                window.camera.x_offset += 1.0f;
                break;

		case GLUT_KEY_PAGE_UP:
				if(window.speedModifier < 2.0f) window.speedModifier += .1f;
				break;
		case GLUT_KEY_PAGE_DOWN:
				if(window.speedModifier > 0.0f) window.speedModifier -= .1f;
				break;

        default:
                cout << "unhandled special key: " << c << endl;
        }
}

void UpdateScene(float current_time) {
        float time = (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused;
        window.camera.Update(time);
		window.splineRotation = (float)((int)(window.splineRotation + 1.0f) % 360);
		window.beatDetectorRotation = (float)((int)(window.beatDetectorRotation + (window.beatDetectorRotationSpeed * window.deltaTime)) % 360);
}

void RenderPassOne(float current_time) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

		vec4 backgroundColor;

		backgroundColor = vec4(0.1f, 0.0f, 0.2f, 1.0f);

	

        ivec2 win_size = ivec2(1024, 768);

        glViewport(0, 0, win_size.x, win_size.y);
        
		//window.background.Draw(win_size, backgroundColor);
        float time = (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused;

        
        // glPolygonMode is NOT modern OpenGL but will be allowed in Projects 2 and 3
        glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);
       
			
		mat4 proj = perspective(50.0f, (float)(1024)/(float)(768), 10.0f, 1000.0f);

		mat4 mv(1.0f);  
		
		mv = lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));	
			
		window.lights.cameraMatrix = mv;
		window.lights.normalMatrix = mat3(inverse(transpose(mv)));
		mv = translate(mv, vec3(0.0, 0.0f, -100.0f));
		mat4 tmp = mv;
		
		tmp = mv;
		tmp = translate(tmp, vec3(0.0f, 0.0f, -100.0f));

		tmp = rotate(tmp, window.beatDetectorRotation, vec3(0, 1, 0));
		tmp = rotate(tmp, -window.beatDetectorRotation, vec3(0, 0, 1));

		
		mv = translate(mv, vec3(-65.0f, 0.0f, 0.0f));
		
		tmp = translate(tmp, vec3(0.0f, 50.0f, 0.0f));
		window.mars.Draw("music_shader", proj, tmp, win_size, window.lights, 30.0f * audio.GetBars(0));
		tmp = translate(tmp, vec3(0.0f, -100.0f, 0.0f));
		window.mars.Draw("music_shader", proj, tmp, win_size, window.lights, 30.0f * audio.GetBars(0));
		
		float weight = 1.0f;
		for (int i = 0; i < 64; i++) {
			float strength = audio.GetBars(i);
			if (strength < .005f) {
				strength = 0.0f;
			}
			float transY = 20.0f * strength * weight;
			mv = translate(mv, vec3(2.0f, transY, 0.0f));
			window.sphere.Draw("music_shader", proj, mv, win_size, window.lights, strength * weight);
			mv = translate(mv, vec3(0.0, -transY, 0.0f));
			weight += 0.3f;
		}

	

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderPassTwo(float current_time) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

		vec4 backgroundColor;

		backgroundColor = vec4(0.1f, 0.0f, 0.2f, 1.0f);

	

        ivec2 win_size = ivec2(1024, 768);

        glViewport(0, 0, win_size.x, win_size.y);
        
		window.background.Draw(win_size, backgroundColor);
        float time = (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused;
		window.deltaTime = time - window.lastFrameTime;
		window.lastFrameTime = time;

        
        // glPolygonMode is NOT modern OpenGL but will be allowed in Projects 2 and 3
        glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);
       
			
		mat4 proj = perspective(50.0f, (float)(1024)/(float)(768), 10.0f, 1000.0f);

		mat4 mv(1.0f);  
		
		mv = lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));	
			
		window.lights.cameraMatrix = mv;
		window.lights.normalMatrix = mat3(inverse(transpose(mv)));
		mv = translate(mv, vec3(0.0, 0.0f, -100.0f));
		
		//mat4 splineMV = mv;
		mat4 tmp = mv;
		
		//splineMV = rotate(splineMV, window.splineRotation, vec3(0, 1, 0));
		//splineMV = rotate(splineMV, window.splineRotation, vec3(0, 0, 1));
		
		//spline.Draw(proj, splineMV, win_size, window.lights, 10);
		
		tmp = translate(tmp, vec3(0.0f, 0.0f, -100.0f));
		
		tmp = rotate(tmp, window.beatDetectorRotation, vec3(0, 1, 0));
		tmp = rotate(tmp, -window.beatDetectorRotation, vec3(0, 0, 1));
		//tmp = rotate(tmp, window.beatDetectorRotation, vec3(1, 0, 0));

		tmp = translate(tmp, vec3(0.0f, 50.0f, 0.0f));
		window.mars.Draw("music_shader", proj, tmp, win_size, window.lights, 30.0f * audio.GetBars(0));
		tmp = translate(tmp, vec3(0.0f, -100.0f, 0.0f));
		window.mars.Draw("music_shader", proj, tmp, win_size, window.lights, 30.0f * audio.GetBars(0));

		mv = translate(mv, vec3(-65.0f, 0.0f, 0.0f));
			
		float weight = 1.0f;
		for (int i = 0; i < 64; i++) {
			float strength = audio.GetBars(i);
			if (strength < .005f) {
				strength = 0.0f;
			}
			float transY = 20.0f * strength * weight;
			mv = translate(mv, vec3(2.0f, transY, 0.0f));
			window.sphere.Draw("music_shader", proj, mv, win_size, window.lights, strength * weight);
			mv = translate(mv, vec3(0.0, -transY, 0.0f));
			weight += 0.3f;
		}

	
		window.beatDetectorRotationSpeed = audio.GetBPMEstimate();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Draw3DSpace(float current_time) {
	ivec2 win_size = ivec2(1024, 768);

	mat4 projection = perspective(25.0f, window.window_aspect, 1.0f, 3000.0f);
	vec3 facing = vec3(0.0f, 0.0f, 0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 position = vec3(0.0f, 0.0f, -4.0f);
	mat4 mv(1.0);
    mat4 temp = mv;

	glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);
	/*mv = scale(mv, vec3(0.11f, 0.11f, 0.11f));
	mv = translate(mv, vec3(0.0, 0.0f, 150.0f));
	mat4 splineMV = mv;*/

	mv = translate(mv, vec3(0.0f, 0.0f, -20.0f));
	mat4 defMV = mv;

	// Sphere with pattern
	mv = scale(mv, vec3(3.5f, 3.5f, 3.5f));
	mv = translate(mv, vec3(-1, -.5f, -1));
	mv = rotate(mv, ((window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused) , vec3(0,1,0));
	//window.mars.Draw("eye_pattern_shader", projection, mv, win_size, window.lights, window.beatDetectorRotation); 

	// post sphere
	mv = defMV;
	mv = translate(mv, vec3(0.0f, 0.0f, 2.0f));
	mv = scale(mv, vec3(1.0f, 1.0f, 1.0f));
	mat4 smallmv = mv;
	mat4 smallmv2 = mv;
	mat4 smallmv3 = mv;
	mat4 smallmv4 = mv;
	mat4 smallmv5 = mv;


	smallmv = rotate(mv, -window.beatDetectorRotation, vec3(1,1,0));
	smallmv = translate(smallmv, vec3(0.1f, -0.2f, 1.0f));
	float scalar = audio.GetBars(0) * 0.25f;
	smallmv = scale(smallmv, vec3(.7f, .7f, .7f));

	//smallmv2 = rotate(mv, window.beatDetectorRotation * 1, vec3(0,1,1));
	smallmv2 = translate(smallmv2, vec3(0.1f, 0.0f, -2.0f + scalar));
	scalar *= 3.0f;
	smallmv2 = scale(smallmv2, vec3(.5f + .9f * scalar, .5f + .9f * scalar, .5f + .9f * scalar));
	scalar /= 3.0f;
	smallmv3 = rotate(mv, window.beatDetectorRotation, vec3(1,1,0));
	smallmv3 = translate(smallmv3, vec3(0.1f, -0.2f, 2.0f));
	smallmv3 = scale(smallmv3, vec3(.3f, .3f, .3f));

	smallmv4 = rotate(mv, -window.beatDetectorRotation, vec3(0,1,1));
	smallmv4 = translate(smallmv4, vec3(0.1f, -0.2f, 3.0f));
	smallmv4 = scale(smallmv4, vec3(.3f, .3f, .3f));
	
	smallmv5 = rotate(mv, window.beatDetectorRotation, vec3(1,0, 0));
	//smallmv5 = rotate(mv, -window.beatDetectorRotation, vec3(1,1, 0));
	smallmv5 = translate(smallmv5, vec3(0.1f, -0.2f, 2.5f));
	smallmv5 = scale(smallmv5, vec3(0.8f, 0.8f, 0.8f));


	hiResSphere.Draw("post_process", projection, smallmv, window.size, window.lights, 0.0f);
	hiResSphere.Draw("screen_pattern_shader", projection, smallmv2, window.size, window.lights, (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused);
	hiResSphere.Draw("texture_shader", projection, smallmv3, window.size, window.lights, window.beatDetectorRotation);
	hiResSphere.Draw("ads_shader", projection, smallmv4, window.size, window.lights, window.beatDetectorRotation);
	hiResSphere.Draw("eye_pattern_shader", projection, smallmv5, win_size, window.lights, 360 * scalar);

	// two splines
	mv = defMV;
	//mv = rotate(mv, 200.0f, vec3(1, 0, 0));
	mv = scale(mv, vec3(.05, .05, .05));
	//mv = rotate(mv, 90.0f, vec3(0,1,0));

	mat4 spline1_mv = mv;
	mat4 spline2_mv = mv;
	spline1_mv = translate(spline1_mv, vec3(0.0f, 0.0f, 10.0f));
	spline1_mv = rotate(spline1_mv, 45.0f, vec3(1,0,0));
	spline1_mv = rotate(spline1_mv, window.beatDetectorRotation, vec3(0,1,0));
	
	spline2_mv = translate(spline2_mv, vec3(0.0f, 0.0f, 10.0f));
	spline2_mv = rotate(spline2_mv, 45.0f, vec3(1,0,0));
	spline2_mv = rotate(spline2_mv, -window.beatDetectorRotation, vec3(0,1,0));
	//mv = defMV;
	
	//spline1_mv = rotate(mv, window.beatDetectorRotation, vec3(0,1,0));
	//spline1_mv = rotate(spline1_mv, window.beatDetectorRotation, vec3(1,1,0));
	
	spline1_mv = scale(spline1_mv, vec3(0.1f, 0.1f, 0.5f + 5.0f * scalar));
	spline2_mv = scale(spline2_mv, vec3(0.2f, 0.2f, 1.5f + 10.0f * scalar));

	
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	ribbon.Draw(projection, spline1_mv, window.size, window.lights, ((window.paused ? window.time_last_pause_began + 45 : current_time + 45) - window.total_time_paused));
	ribbon2.Draw(projection, spline2_mv, window.size, window.lights, ((window.paused ? window.time_last_pause_began + 45 : current_time + 45) - window.total_time_paused));
	//ribbon.Draw(projection, spline2_mv, window.size, window.lights, ((window.paused ? window.time_last_pause_began + 180: current_time + 180) - window.total_time_paused));
	glDisable(GL_BLEND);

	
}

void PostProcess(float current_time) {
        UpdateScene(current_time);
		//spline.Update(window.deltaTime);
		
		ivec2 win_size = ivec2(1024, 768);

        glEnable(GL_CULL_FACE);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, window.size.x, window.size.y);
        
        float time = (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused;
        mat4 projection = perspective(25.0f, (float)(1024/768), 1.0f, 3000.0f); //fixed value because of our texture viewport
        vec3 facing = vec3(0.0f, 0.0f, 0.0f);
        vec3 up = vec3(0.0f, 1.0f, 0.0f);
        vec3 position = vec3(0.0f, 0.0f, -4.0f);
        mat4 view = lookAt(position, facing, up);
		mat4 mv = view;
		mat4 temp = view;
        
		//view = rotate(view, 180.0f, vec3(0, 0, 0));
        view = scale(view, vec3(0.11f, 0.11f, 0.10f));
        view = translate(view, vec3(-50.0f, -50.0f, 200.0f));
        
		window.rendertexture.Draw(1, 2, projection, view, window.size, window.lights, (window.paused ? window.time_last_pause_began : current_time) - window.total_time_paused);  

		Draw3DSpace(current_time);

		glFlush();
}

void DisplayFunc()
{
        float current_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
		if (!window.paused) {
			bool beat = audio.DrawScene();
		}

        //Render scene normally to texture for post-processing

		glActiveTexture(GL_TEXTURE0); //for whatever reason, this is necessary
        glBindTexture(GL_TEXTURE_2D, 0);
		glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		
		window.fboSecondary.Use();

        RenderPassOne(current_time);

		window.fboSecondary.Disable();
	
		window.frame_buffer.Use();

		RenderPassTwo(current_time);

		window.frame_buffer.Disable();

        //Finally, render texture to screen using post-processing shaders
        PostProcess(current_time);
        
}

void TimerFunc(int value)
{
        // Question for reader: Why is this 'if' statement here?
        if (window.window_handle != -1)
        {
                glutTimerFunc(window.interval, TimerFunc, value);
                glutPostRedisplay();
        }
}

int main(int argc, char * argv[])
{
        glutInit(&argc, argv);
        glutInitWindowSize(1024, 768);
        window.size = vec2(1024, 768);
        window.window_aspect = 1024 / 768;

        glutInitWindowPosition(0, 0);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);

        window.window_handle = glutCreateWindow("Bars");
        glutReshapeFunc(ReshapeFunc);
        glutCloseFunc(CloseFunc);
        glutDisplayFunc(DisplayFunc);
        glutKeyboardFunc(KeyboardFunc);
        glutSpecialFunc(SpecialFunc);
        glutTimerFunc(window.interval, TimerFunc, 0);
        glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

        ilInit();

        if (glewInit() != GLEW_OK)
        {
                cerr << "GLEW failed to initialize." << endl;
                return 0;
        }

        if (!window.background.Initialize())
                return 0;

        char *default_mars;

        default_mars = "mars4.txt";

        if(!window.mars.Initialize((float)window.slices, default_mars, "mars.jpg"))
                return 0;
		
		 if(!window.sphere.Initialize((float)window.slices, default_mars, "mars.jpg"))
                return 0;

        if(!window.camera.Initialize()) {
                return 0;
        }
        if(!window.rendertexture.Initialize()) {
                return 0;
        }
		if(!ribbon.Initialize(100, ivec2(40, 20), ivec2(10, 5), ivec2(1, 0))) {
			return 0;
		}

		if(!ribbon2.Initialize(100, ivec2(4, 2), ivec2(4, 2), ivec2(10, 5))) {
			return 0;
		}

		if(!hiResSphere.Initialize(100, "large", "nil"))
			return 0;

        Light light, spotlight;
        light.SetPosition(vec4(0.0f, 0.0f, 50.0f, 1.0f));

        window.lights.Add(light);


        spotlight.SetPosition(vec4(0.0f, 0.0f, 0.0f, 1.0f));
        spotlight.direction = vec3(0.0f, -0.1f, -1.0f);
        window.lights.Add(spotlight);

        window.frame_buffer.Initialize(window.size.x, window.size.y, 1);
		window.fboSecondary.Initialize(window.size.x, window.size.y, 2);

		

		audio.Initialize();
		
        glutMainLoop();
}