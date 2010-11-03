This is libSDL 1.2 and 1.3 ported to Google Android (also bunch of other libs included).
Alien Blaster game is used as working example (original sources: http://www.schwardtnet.de/alienblaster/ ).

Installation
============

This should be compiled with Android 2.2 SDK and NDK r4b - google for them and install them as described in their docs
(the application will run on Android 1.6 and above).
You'll need to install Ant too.
The most supported environnment for that port is Linux, MacOs should be okay too, 
however if you'll use launchConfigure.sh script you'll have to replace "linux-x86" to "darwin-x86" inside it.
If you're developing under Windows you'd better install andLinux or Ubuntu+Wubi, to get proper Linux environment
running inside Windows, then install Linux toolchain on it. I was told andLinux compiles faster than Cygwin.
Also you'll need full set of Linux utils and symlinks support to launch ChangeAppSettings.sh (sh, grep, sed, tr).

How to compile Alien Blaster demo application
=============================================

Go to "project" directory and launch command
	android update project -p .
Then go back, edit file build.sh if needed to add NDK dir to your PATH, then launch it.
It will compile a bunch of libs under project/libs/armeabi,
create file project/bin/DemoActivity-debug.apk and install it on your device or emulator.
Then you can test it by launching Alien Blaster icon from Android applications menu.
It's designed for 640x480, so if you have smaller screen it will be resized.
Note: The game enforces horizontal screen orientation, you may open your keyboard and use it for
additional keys - the phone will just keep current screen orientation.
Recent Android phone models like HTC Evo have no keyboard at all, on-screen keyboard built into libSDL
is available for such devices (it's rather limited yet, only joystick and 7 keys, no text input).

This port also supports GL ES + SDL combo - there is GLXGears demo app in project/jni/application/glxgears,
remove project/jni/application/src symlink and make new one pointing to glxgears, 
also you'll have to enable Z-Buffer in ChangeAppSettings.sh.
Note that GL ES is NOT pure OpenGL - there are no glBegin() and glEnd() call and other widely used functions,
and generally it will take a lot of effort to port pure OpenGL application to GL ES.

How to compile your own application
===================================

When porting you own app, first of all ensure that your application supports
native RGB_565 pixel format and AUDIO_S8 or AUDIO_S16 audio format
(there is RGB_565 pixelformat even for OpenGL, not BGR_565 as all other OpenGL implementation have).
Colorkey images are supported using RGBA_5551 pixelformat with 1-bit alpha (SDL does conversion internally,
for you they are just RGB_565 surfaces), alpha surfaces have RGBA_4444 format. 
See file project/jni/application/alienblaster/SdlForwardCompat.h
to learn how to make your application use SDL 1.3 instead of SDL 1.2 without much pain.
HTC G1/Nexus One has native screen resolution 480x320, HTC Evo has 800x480, you may toggle automatic
screen resizing in ChangeAppSettings.sh and draw to virtual 640x480 screen - 
it will be HW accelerated and will not impact performance much.
SDL_ListModes()[0] will always return native screen resolution.
Also make sure that your HW textures are not wider than 1024 pixels, or it will fail to allocate such
texture on HTC G1. Software surfaces may be of any size of course (but you don't want to do expensive memcpy).

Alternatively, SDL 1.2 is available too, you may use it with SW video as usual, however if you want HW acceleration
there are few restrictions: you cannot currently blit SW surface to screen, it should be only HW surface,
however you can use colorkey, per-surface alpha and per-pixel alpha.
Also the screen is always double-buffered, and after each SDL_Flip() there is garbage in pixel buffer,
so forget about dirty rects and partial screen updates - you have to re-render whole picture each frame.
Single-buffer rendering might be possible with techniques like glFramebufferTexture2D(),
however it may not be present on all devices, so I won't do that.
Basically your code should be like:

// Init HW-accelerated video
SDL_SetVideoMode( 640, 480, 16, SDL_DOUBLEBUF | SDL_HWSURFACE );

// Load graphics
SDL_Surface *sprite = IMG_Load( "sprite.png" );
SDL_Surface * hwSprite;

if( sprite->format->Amask )
{
	// Surface contains per-pixel alpha, convert it to HW-accelerated format
	hwSprite = SDL_DisplayFormatAlpha(sprite);
}
else
{
	// Set pink color as transparent
	SDL_SetColorKey( sprite, SDL_SRCCOLORKEY, SDL_MapRGB(sprite->format, 255, 0, 255) );
	// Create HW-accelerated surface
	hwSprite = SDL_DisplayFormat(sprite);
	// Set per-surface alpha, if necessary
	SDL_SetAlpha( hwSprite, SDL_SRCALPHA, 128 );
}
SDL_FreeSurface(sprite);

// Blit it in HW-accelerated way
SDL_BlitSurface(hwSprite, sourceRect, SDL_GetVideoSurface(), &targetRect);

// Wrong, blitting SW surfaces to screen not supported
SDL_BlitSurface(sprite, sourceRect, SDL_GetVideoSurface(), &targetRect);

// Wrong, copying from video surface not supported
SDL_BlitSurface(SDL_GetVideoSurface(), sourceRect, sprite, &targetRect);
// In the future I may add implementation to read screen buffer with glReadPixels(), however it will be slow (okay for screenshots).

To compile your own app, put your app sources into project/jni/application dir (or create symlink to them), 
and change symlink "src" to point to your app:

	cp -r /path/to/my/app project/jni/application/myapp
or
	ln -s /path/to/my/app project/jni/application/myapp
then
	rm project/jni/application/src
	ln -s myapp project/jni/application/src
(the second one should be relative link without slashes)

Then launch script ChangeAppSettings.sh - it will ask few questions and modify some Java code.
You may take AndroidAppSettings.cfg file from some other application to get some sane defaults,
you may launch ChangeAppSettings.sh with -a or -v parameter to skip questions altogether or to ask only version code.
The C++ files shall have .cpp extension to be compiled, rename them if necessary.
Also you can replace icon image at project/res/drawable/icon.png and image project/res/drawable/publisherlogo.png.
Then you can launch build.sh.

The NDK has RTTI and exceptions disabled for C++ code, if you need them you may download modified NDK from
http://www.crystax.net/android/ndk-r4.php - note however that you cannot throw exceptions across shared library boundary.
Unzip it, and put in your PATH instead of original NDK - do not rename the target dir, my makefiles will
check if there's "crystax" string in path to gcc toolchain, and will disable STLPort because CrystaX's
NDK already contains STL library.

Application data may be bundled with app itself, or downloaded from net on first run.
Create .ZIP file with your application data, and put it on HTTP server, or to "project/jni/application/src/AndroidData" dir - 
ChangeAppSettings.sh will ask you for the URL, if URL won't contain "http://" it will try to unzip file from AndroidData dir.
Note that there is limit on maximum .APK file size on Market, like 20 Mb or so, so big files should be downloaded by HTTP.
If you'll release new version of data files you should change download URL or data file name and update your app as well -
the app will re-download the data if URL does not match the saved URL from previous download.

If you'll add new libs - add them to project/jni/, copy Android.mk from existing lib, and
add libname to project/jni/<yourapp>/Android.mk
If lib contains "configure" script - go to lib dir and execute command "../launchConfigureLib.sh" - it will
launch "configure" with appropriate environment and will create the "config.h" file at least, though linking
will most probably fail because of ranlib - just edit Android.mk to compile lib sources and remove all tools and tests.

The ARM architecture has some limitations which you have to be aware about -
if you'll access integer that's not 4-byte aligned you'll get garbage instead of correct value,
and it's processor-model specific - it may work on some devices and do not work on another ones -
you may wish to check your code in Android 1.6 emulator from time to time to catch such bugs.

char * p = 0x13; // Non-4 byte aligned pointer
int i = (int *) p; // We have garbage inside i now
memcpy( &i, p, sizeof(int) ); // The correct way to dereference a non-aligned pointer

This compiler flags will catch most obvious errors, you may add them to AppCflags var in settings:
-Werror=strict-aliasing -Werror=cast-align -Werror=pointer-arith -Werror=address

How to compile your own application using automake/configure scripts
====================================================================

There is limited support for "configure" scripts, I've managed to compile lbreakout2 that way,
though ./configure scripts tend to have stupid bugs in various places, avoid using that method if you can.
1. Download lbreakout2-2.6.1.tar.gz from http://lgames.sourceforge.net/, unpack it to project/jni/application dir.
2. Determine libraries needed for your app, launch ChangeAppSettings.sh, select correct libSDL version
   (1.2 for lbreakout2), and correct libs (sdl_mixer sdl_image sdl_net for lbreakout2), also change name etc.
3. Launch ./build.sh, wait till it builds all .so files
4. Go to project/jni/application/lbreakout2-2.6.1 dir, and launch command
   ../launchConfigure.sh --disable-install --enable-sdl-net LIBS=-lintl
5. Watch how ./configure configures, if it fails fix launchConfigure.sh, rinse and repeat.
6. Launch make, and pray. If you're lucky it will create application binary (lbreakout2-2.6.1/client/lbreakout2)
7. Move the application binary to dir project/libs/armeabi, rename it to libapplication.so (overwrite old file)
8. Run command "arm-eabi-strip -g libapplication.so", you can find arm-eabi-strip under your NDK dir.
9. Run "ant debug" or "ant release" from project dir, install to device & enjoy.

Android Application lifecycle support
=====================================

Application may be put to background at any time, for example if user gets phone call onto the device.
The application will lose OpenGL context then, and has to re-create it when put to foreground.

The SDL provides function 
SDL_ANDROID_SetApplicationPutToBackgroundCallback( callback_t appPutToBackground, callback_t appRestored );
where callback_t is function pointer of type "void (*) void".
The default callbacks will call another Android-specific functions:
SDL_ANDROID_PauseAudioPlayback() and SDL_ANDROID_ResumeAudioPlayback()
which will pause and resume audio from HW layer, so appplication does not need to destroy and re-init audio.
Also, the usual event SDL_ACTIVEEVENT with flag SDL_APPACTIVE will be sent when that happens.

If you're using pure SDL 1.2 API (with or without HW acceleration) you don't need to worry about anything -
the SDL itself will re-create GL textures and fill them with pixel data from existing SDL HW surfaces,
so you may leave the callbacks to defaults.

If you're using SDL 1.3 API and using SDL_Texture, then the textures pixeldata is lost - you will need 
to call SDL_UpdateTexture() to refill texture pixeldata from appRestored() callback for all your textures.
If you're using compatibility API with SDL_Surfaces you don't have to worry about that.

If you're using SDL with OpenGL with either SDL 1.2 or SDL 1.3, the situation is even more grim -
not only all your GL textures are lost, but all GL matrices, blend modes, etc. has to be re-created.

OS may decide there's too little free RAM left on device, and kill background applications 
without notice, so it vill be good to create temporary savegame etc. from appPutToBackground() callback.

Also it's a good practice to pause any application audio, especially if the user gets phone call,
and if you won't set your own callbacks the default callbacks will do exactly that.
There are circumstances when you want to avoid that, for example if the application is audio player,
or if application gets some notification over network (for example you're running a game server,
and want a beep when someone connects to you) - you may unpause audio for some short time,
that will require another thread to watch the network, because main thread will be blocked inside SDL_Flip().

The application is not allowed to do any GFX output without OpenGL context (or it will crash),
that's why SDL_Flip() call will block until we're re-acquired context, and the callbacks will be called
from inside SDL_Flip().
The whole idea behind callbacks is that the existing application should not be modified to
operate correctly - the whole time in background will just look to app as one very long SDL_Flip(),
so it's good idea to implement some maximum time cap on game frame, so it won't process
the game to the end level 'till the app is in background, or calculate the difference in time
between appPutToBackground() and appRestored() and update game time variables.

Alternatively, you may enable option for unblocked SDL_Flip() in ChangeAppSettings script,
then you'll have to implement special event loop right after each SDL_Flip() call:

SDL_Flip();
SDL_Event evt;
while( SDL_PollEvent(&evt) )
{
	if( evt.type == SDL_ACTIVEEVENT->SDL_APPACTIVE && evt.active.gain == 0 && evt.active.state == SDL_APPACTIVE )
	{
		// We've lost GL context, we are not allowed to do any GFX output here, or app will crash!
		while( 1 )
		{
			SDL_PollEvent(&evt);
			if( evt.type == SDL_ACTIVEEVENT->SDL_APPACTIVE && evt.active.gain && evt.active.state == SDL_APPACTIVE )
			{
				SDL_Flip(); // One SDL_Flip() call is required here to restore OpenGL context
				// Re-load all textures, matrixes and all other GL states if we're in SDL+OpenGL mode
				// Re-load all images to SDL_Texture if we're using it
				// Now we can draw
				break;
			}
			
			// Process network stuff, maybe play some sounds using SDL_ANDROID_PauseAudioPlayback() / SDL_ANDROID_ResumeAudioPlayback()
			SDL_Sleep(200);
		}
	}
}

Note that I did not test that code yet, so test reports are appreciated.

Quick guide to debug native code
================================

To debug your application add tag 'android:debuggable="true"' to 'application' element in AndroidManifest.xml,
recmpile and reinstall your app to Android 2.2 emulator or Android 2.2 device, go to "project" dir and launch command
	ndk-gdb --verbose --start --force
then when it fails enter command
	target remote:5039 (then it will fail again)
Note that it's extremely buggy, and I had no any success in debugging my app with ndk-gdb 
(I was trying with NDK r4, NDK r4b might work better).
So it's best to debug with code like:
	__android_log_print(ANDROID_LOG_INFO, "My App", "We somehow reached execution point #224");
and then watching "adb logcat" output.

If your application crashed, you should use following steps:

1. Gather the crash report from "adb logcat" - it should contain stack trace, if it does not then you're unlucky,

I/DEBUG   (   51): *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
I/DEBUG   (   51): Build fingerprint: 'sprint/htc_supersonic/supersonic/supersonic:2.1-update1/ERE27/194487:userdebug/test-keys'
I/DEBUG   (   51): pid: 915, tid: 924  >>> de.schwardtnet.alienblaster <<<
I/DEBUG   (   51): signal 11 (SIGSEGV), fault addr deadbaad
I/DEBUG   (   51):  r0 00000000  r1 afe133f1  r2 00000027  r3 00000058
I/DEBUG   (   51):  r4 afe3ae08  r5 00000000  r6 00000000  r7 70477020
I/DEBUG   (   51):  r8 000000b0  r9 ffffff20  10 48552868  fp 00000234
I/DEBUG   (   51):  ip 00002ee4  sp 485527f8  lr deadbaad  pc afe10aac  cpsr 60000030
I/DEBUG   (   51):          #00  pc 00010aac  /system/lib/libc.so
I/DEBUG   (   51):          #01  pc 0000c00e  /system/lib/libc.so
I/DEBUG   (   51):          #02  pc 0000c0a4  /system/lib/libc.so
I/DEBUG   (   51):          #03  pc 0002ca00  /data/data/de.schwardtnet.alienblaster/lib/libsdl.so
I/DEBUG   (   51):          #04  pc 00028b6e  /data/data/de.schwardtnet.alienblaster/lib/libsdl.so
I/DEBUG   (   51):          #05  pc 0002d080  /data/data/de.schwardtnet.alienblaster/lib/libsdl.so

2. Go to project/bin/ndk/local/armeabi dir, find there the library mentioned in stacktrace
(libsdl.so in our example), copy the address of the first line of stacktrace (0002ca00), and execute command

gdb libsdl.so -ex "list *0x0002ca00"

It will output the exact line in your source where the application crashed.

License information
===================

The libSDL port itself is licensed under LGPL, so you may use it for commercial app without releasing sources,
however you'll have to release the file ChangeAppSettings.sh to allow linking newer version of libSDL with
your compiled application, as LGPL requires.
It contains separate libraries under project/jni, each of which has it's own license,
I've tried to compile all LGPL-ed libs as shared libs but you should anyway inspect the licenses
of the libraries you're linking to if you're creating closed-source app.
libMAD is licensed under GPL, so if you're planning to make commercial app you should avoid using it,
otherwise you'll have to release your application sources under GPL too.

The "Ultimate Droid" button theme by Sean Stieber is licensed under Creative Commons - Attribution license.
