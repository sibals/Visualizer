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
#include "Spline.h"

using namespace std;
using namespace glm;

Window window;
GLuint whiteTexHandle;
SoundManager audio; 
Mars* spheres = new Mars[64];
Spline spline;

void CloseFunc()
{
        window.window_handle = -1;
        window.background.TakeDown();
        window.mars.TakeDown();
        window.starfield.TakeDown();
        window.rendertexture.TakeDown();
		spline.TakeDown();
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
        case 'V':
        case 'v':
                window.camera.SetCameraType((window.camera.GetCameraType() == Camera::Type::normal) ? Camera::Type::chase : Camera::Type::normal);
                break;
        case 'Z':
        case 'z':
                break;
        case 'S':
        case 's':
                if (window.mars.cutoff_angle == 0.0f) {
                        window.mars.cutoff_angle = 15.0f;
                } else {
                        window.mars.cutoff_angle = 0.0f;
                }
                break;
        case 'Q':
        case 'q':
                window.rendertexture.StepPostEffect();
                break;
        case 'N':
        case 'n':
                window.mars.EnableNormals(window.normals);
                break;        
        case 'W':
                window.mars.wireframe_mode++;
                if(window.mars.wireframe_mode > 3)
                        window.mars.wireframe_mode = 0;
                break;
        case 'w':
                window.wireframe = !window.wireframe;
                break;
        case 'o':
        case 'O':
                window.draw_planes = !window.draw_planes;
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
			spheres[i].Draw("music_shader", proj, mv, win_size, window.lights, strength * weight);
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
			spheres[i].Draw("music_shader", proj, mv, win_size, window.lights, strength * weight);
			mv = translate(mv, vec3(0.0, -transY, 0.0f));
			weight += 0.3f;
		}

	
		window.beatDetectorRotationSpeed = audio.GetBPMEstimate();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void PostProcess(float current_time) {
        UpdateScene(current_time);
		spline.Update(window.deltaTime);
		
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
        
		projection = perspective(25.0f, window.window_aspect, 1.0f, 3000.0f);

		glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);
		mv = scale(mv, vec3(0.11f, 0.11f, 0.11f));
		mv = translate(mv, vec3(0.0, 0.0f, 150.0f));
		mat4 splineMV = mv;
		
		//splineMV = rotate(splineMV, window.splineRotation, vec3(0, 1, 0));
		//splineMV = rotate(splineMV, window.splineRotation, vec3(0, 0, 1));
		splineMV = translate(splineMV, vec3(window.splineRotation / 10.0, 0.0f, 0.0f));
		spline.Draw(projection, splineMV, win_size, window.lights, 10);

		temp = translate(temp, vec3(0.0f, 0.0f, 15.0f));
		temp = scale(temp, vec3(.25f, .25f, .25f));
		window.mars.Draw("music_shader", projection, temp, win_size, window.lights, 10); 
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
        if(!window.camera.Initialize()) {
                return 0;
        }
        if(!window.starfield.Initialize()) {
                return 0;
        }
        if(!window.rendertexture.Initialize()) {
                return 0;
        }
		spline.Initialize(5, vec3(-50.0f, 0.0f, 0.0f), vec3(50.0f, 0.0f, 0.0f)); 

        Light light, spotlight;
        light.SetPosition(vec4(0.0f, 0.0f, 50.0f, 1.0f));

        window.lights.Add(light);


        spotlight.SetPosition(vec4(0.0f, 0.0f, 0.0f, 1.0f));
        spotlight.direction = vec3(0.0f, -0.1f, -1.0f);
        window.lights.Add(spotlight);

        window.frame_buffer.Initialize(window.size.x, window.size.y, 1);
		window.fboSecondary.Initialize(window.size.x, window.size.y, 2);

		for (int i = 0; i < 64; i++) {
			spheres[i] = window.mars;
		}

		audio.Initialize();
		
        glutMainLoop();
}