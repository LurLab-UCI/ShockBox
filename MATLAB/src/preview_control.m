function  preview_control(app)
global arduino camera_running vid;

if camera_running
    write(arduino,')',"char");
    camera_running = 0;
    stop(vid);
    closepreview(vid);
    app.start_camera.Text = 'Start Eye Camera';
else
    frate = round(1000/app.framerate.Value/2);
    write(arduino,'(',"char");
    write(arduino,frate,"uint8");
    camera_running = 1;
    app.start_camera.Text = 'Stop Eye Camera';
    preview_Callback_n(app)    
end
            