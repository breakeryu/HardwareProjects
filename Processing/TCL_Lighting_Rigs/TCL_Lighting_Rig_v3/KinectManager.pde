import org.openkinect.*;
import org.openkinect.processing.*;

// Internal flob tracking to get rough position data:
import s373.flob.*; // http://s373.net/code/flob/

class KinectManager {
	PApplet parent;

	// Keep track of how many kinects are alive automatically:
	int kinectCount;

	// Arrays for holding each devices info:
	Kinect[] kinectDevices;
	PImage[] kinectDisplays;

	// Size of kinect image
	int kw = 640;
	int kh = 480;

	// Flob tracking config:
	Flob[] flobTrackers; // An array of flob trackers, one for each Kinect
	boolean flobTrackingEnabled = true;
	ArrayList[] kinectBlobs;
	float fade;
	int minSize = 200;
	int maxSize = 29000;
  	float threshold = 150; // Flob library's Kinect depth threshold. range is 0-255

  	// Depth parseing for hallways:
  	PImage emptyDepths;

	KinectManager(PApplet parent_) {
		parent = parent_;

		kinectCount = Context.getContext().devices();

		kinectDevices = new Kinect[kinectCount];
		kinectDisplays = new PImage[kinectCount];

		if (flobTrackingEnabled) {
			flobTrackers = new Flob[kinectCount];
			kinectBlobs = new ArrayList[kinectCount];
			emptyDepths = new PImage();
		}

		if (kinectCount>0) {
			println(kinectCount+" Kinect(s) attached.");
			println("Initializing them all...");
			for (int i = 0; i<kinectCount; i++) {
				kinectDevices[i] = new Kinect(parent);
			    kinectDevices[i].start(i);
			    kinectDevices[i].enableRGB(true);
			    // kinectDevices[i].enableIR(false); // CAN NOT be uncommented if we want to use RGB
			    kinectDevices[i].enableDepth(true);
			    // kinectDevices[i].tilt(-60);
			    kinectDevices[i].processDepthImage(true);

			    kinectDisplays[i] = kinectDevices[i].getDepthImage();
			    if (flobTrackingEnabled) {
					flobTrackers[i] = initalizeFlobTracker(kinectDisplays[i]);
					kinectBlobs[i] = new ArrayList(); // Array of the blobs detected
				}
			    
			}
		} else {
			println("No Kinects were found! Exiting...");
			exit();
		}
	}

	void quit() {
		println("Disconnecting all attached Kinects");
		if (kinectCount>0) {
			for (int i = 0; i<kinectCount; i++) {
				kinectDevices[i].quit();
			}
		}
	}

	//===============================
	// Flob tracking related methods:

	// This method will parse all of the Kinect depthImages for Flobs:
	void parse() {
		// i = kinect index
		for (int i = 0; i<kinectCount; i++) {
			// TBlob stuff:
			// Copy the depth image into the flob tracker:
			kinectDisplays[i] = kinectDevices[i].getDepthImage();

			// Copy the depth image into the flob tracker but resize it while doing so:
			// kinectDisplays[i].copy(kinectDevices[i].getDepthImage(), 0, 0, kw, kh, 0, 0, kw/2, kh/2);

		    // Find the blobs from the image:
		    kinectBlobs[i] = flobTrackers[i].track( flobTrackers[i].binarize(kinectDisplays[i]) );


		    // ABlob stuff:
		    // kinectBlobs[i] = flobTrackers[i].calc( flobTrackers[i].binarize(kinectDisplays[i]) );


		    // Custom depth filtering:
		    // background(100);

		 //    int[] depth = kinectDevices[i].getRawDepth();
		 //    kinectDisplays[i] = kinectDevices[i].getVideoImage();
		 //    PImage rawDepth = createImage(kinectDisplays[i].width, kinectDisplays[i].height, ALPHA);
		 //    // rawDepth.copy(kinectDevices[i].getDepthImage(), 0, 0, kw, kh, 0, 0, kw, kh);
			// // image(rawDepth, 0, 0);

			// // // // ToDo: Add some more image processing so we can control how the depth is parsed:
			// rawDepth.loadPixels();
			
			// // // j = pixel index
			// // println(kinectDisplays[i].width * kinectDisplays[i].height);
			// for (int j = 0; j < (rawDepth.width * rawDepth.height); j += 1) {
			// 	if (j > (rawDepth.width * rawDepth.height)/2 ) {
			// 		rawDepth.pixels[j] = color(0);
			// 	} else {
			// 		rawDepth.pixels[j] = color(100);
			// 	}
			// }

			// rawDepth.updatePixels();

			// // image(rawDepth, 0, 0);

			// // Find the blobs from the image:
			// // kinectBlobs[i] = flobTrackers[i].track( rawDepth );
		 //    kinectBlobs[i] = flobTrackers[i].track( flobTrackers[i].binarize(rawDepth) );

		}
	}

	// Draw the kinect image or the flob src image:
	void draw() {
		// for (int i = 0; i<kinectCount; i++) {
		// 	kinectDisplays[i] = kinectDevices[i].getDepthImage();
		// 	image(kinectDisplays[i], i*kw, 0);
		// }

		background(100);
	  	imageMode(CORNER);
		image(flobTrackers[0].getSrcImage(), 0, 0);
	}

	void drawDebug(int kinectDevice) {
		// background(100);
		// imageMode(CORNER);
		// image(flobTrackers[kinectDevice].getSrcImage(), 0, 0);

		rectMode(CENTER);
		for(int i = 0; i < kinectBlobs[kinectDevice].size(); i++) {
			// TBlob stuff:
			TBlob tb = flobTrackers[kinectDevice].getTBlob(i);

			// Draw blob outline:
			fill(220, 220, 255, 100);
			rect(tb.cx, tb.cy, tb.dimx, tb.dimy);

			// Draw blob center:
			fill(0, 255, 0);
			rect(tb.cx, tb.cy, 10, 10);

			// Draw velocity line:
			// fill(0,255,0);
			// float velmult = 100.0f;
			// line(tb.cx, tb.cy, tb.cx + tb.velx * velmult ,tb.cy + tb.vely * velmult ); 

			// Draw blob info text:
			fill(255, 0, 0);
			String txt = "id: " + tb.id + " time live: " + (tb.presencetime);
			text(txt, tb.cx + 10f, tb.cy + 5f);   

			// ABlob stuff:
		    // skeleton data
		    // ABlob ab = flobTrackers[kinectDevice].getABlob(i);
		    // line(ab.cx, ab.cy, ab.armleftx, ab.armlefty);
		    // line(ab.cx, ab.cy, ab.armrightx, ab.armrighty);
		    // line(ab.cx, ab.cy, ab.headx, ab.heady);
		    // line(ab.cx, ab.cy, ab.bottomx, ab.bottomy);
		    // line(ab.cx, ab.cy, ab.footleftx, ab.footlefty);
		    // line(ab.cx, ab.cy, ab.footrightx, ab.footrighty);
		}
		rectMode(CORNER);

		fill(127,255,0);
		text("Threshold: " + getThreshold() + "     ] increase threshold, [DOWN] decrease threshold", 5, height - 15);
		text("min: " + getMinBlob() + " max: " + getMaxBlob(), 5, height - 5);
	}

	//=========================
	// Getters and setters:

	// This method will return a specific blob from the blob tracker attached for a specific Kinect:
	// kinectDevice is an integer: 0 for the first Kinect, 1 for the second, etc.
	//    blobIndex is an integer: 0 for the first blob
	PVector getBlob(int kinectDevice, int blobIndex) {
		if (kinectBlobs[kinectDevice].size()>0) {
			TBlob tb = flobTrackers[kinectDevice].getTBlob(blobIndex);
			return new PVector(tb.cx, tb.cy);
		}
		return null;
	}

	void setBackground(PImage flobBackground) {
		for (int i = 0; i < kinectCount; i++) {
			// flobTrackers[i].setThresholdmode(1);
			flobTrackers[i].setBackground(flobBackground);
		}
	}
	void resetBackground() {
		// PImage blankImage = new PImage();
		// blankImage.loadPixels();
		// for (int i = 0; i < (blankImage.width * blankImage.height); i++) {
		// 	blankImage.pixels[i] = color(0);
		// }
		// blankImage.updatePixels();

		for (int i = 0; i < kinectCount; i++) {

			int[] depth = kinectDevices[i].getRawDepth();
		    PImage rawDepth = createImage(kinectDisplays[i].width, kinectDisplays[i].height, ALPHA);

		    rawDepth.loadPixels();
		    // println("Depth array is this big: " + depth.length);
		    // println("Background image is this big: " + kinectDisplays[i].width*kinectDisplays[i].height);
		    for (int j = 0; j < (kinectDisplays[i].width*kinectDisplays[i].height); j +=1) {
		    	if (depth[j] < 130) {
		    		rawDepth.pixels[j] = 0;
		    	} else {
		    		rawDepth.pixels[j] = depth[j]+10;
		    	}
		    	
		    }
		    rawDepth.updatePixels();

		    image(rawDepth, 0, 0);

			// kinectDisplays[i] = kinectDevices[i].getDepthImage();
			// flobTrackers[i].setThresholdmode(2);
			// flobTrackers[i].setBackground(kinectDevices[i].getDepthImage());
			// flobTrackers[i].easeBackground(kinectDevices[i].getDepthImage());
			// flobTrackers[i].easeBackground(kinectDevices[i].getDepthImage());
			flobTrackers[i].setBackground(rawDepth);
		}
	}

	float getThreshold() {
		return flobTrackers[0].getThresh();
	}
	void setThreshold(float newThresh) {
		threshold = newThresh;
		for (int i = 0; i < kinectCount; i++) {
			flobTrackers[i].setThresh(threshold);
		}
	}

	int getMinBlob() {
		return flobTrackers[0].getMinNumPixels();
	}
	int getMaxBlob() {
		return flobTrackers[0].getMaxNumPixels();
	}

	void setMinBlob(int count) {
		minSize = count;
		for (int i = 0; i < kinectCount; i++) {
			flobTrackers[i].setMinNumPixels(minSize);
		}
	}
	void setMaxBlob(int count) {
		maxSize = count;
		for (int i = 0; i < kinectCount; i++) {
			flobTrackers[i].setMaxNumPixels(maxSize);
		}
	}

	// End getters and setters
	//=========================

	//============
	// Flob Init:

	// This method returns a single flob tracker instance. The provided PImage flobBackground is used as the trackers background image:
	Flob initalizeFlobTracker(PImage flobBackground) {
		// Flob detection setup:
		// flob = new Flob(parent, kw/2, kh/2, kw, kh); // Use HALVED Kinect video size for the maximum possible blob positions
	    Flob flob = new Flob(parent, kw, kh, kw, kh); // Use the FULL Kinect video size for the maximum possible blob positions
	    // flob = new Flob(parent, display); // Use the full window for blob positions

	    // Set the background image (the image has been created but it is black at this point):
	    // flob.setBackground(flobBackground);

	    // Set the internal blur functions amount (0=off, 5=strong blur):
	    // Note: This is off because it's slow as hell.
	    flob.setBlur(0);

	    // Set Flob detection settings:
	    // Set operating mode to CONTINUOUS_DIFFERENCE (1) or STATIC_DIFFERENCE (0) or CONTINUOUS_EASE_DIFFERENCE (2)
	    flob.setOm(0);

	    // Fade amount (only used when operating mode is >0):
	    // fade = 25;  
	    // flob.setFade(fade);

	    // Set the gray color value for threshold mods >0:
	    flob.setThresh(threshold);
	    //set @thresholdmode 
	    // - @flob.ABS (0): absolute diference of incoming pixel versus background 
	    // - @flob.LESSER (1): if incomingpixel less than threshold, mark as white pixel in binary image
	    // - @flob.GREATER (2): white if above @videothresh value
	    // flob.setThresholdmode(0); 
	    
	    // This variable sets the blob source image mode:
	    // set the videotex returned by flob.videotex 
	    // case 0: videotex = src videoimg as flob sees it (incoming image)
	    // case 1: videotex = binary image result from om==0, incoming img vs static bg
	    // case 2: videotex = binary image result from om>0, incoming img vs dynamic bg
	    // case 3: videotex = image result from incoming img + binary image
	    flob.setSrcImage(3);

	    // Set the minimum distance between pixels that will be considered a single blob:
	    // flob.setTrackingMinDist(10); 

	    // Set the length of time a blob will be tracked for past it's death:
	    // flob.setTBlobLifeTime(25);

	    // Set the size limits of a blob (in pixels):
	    flob.setMinNumPixels(minSize);
	    flob.setMaxNumPixels(maxSize);

	    // Turn on the flob libraries Kinect gray clamping mode:
	    // THIS DOESN'T FUCKING WORK!!! WOO!
	    // flob.setClampGray(true);
	    // flob.setNearGray(grayNear); // This should be < setFarGray
	    // flob.setFarGray(grayFar);  // This should be > setNearGray
	    // flob.setClampGray(true).setNearGray(10).setFarGray(80);


	    // Try doing some feature parseing
	    // Damnit. This only works because of blob shape.
	   //  boolean feats[] = {true,true,true,true,true};
  		// flob.setTrackFeatures( feats );

	    println("Max num pixels: " + flob.getMaxNumPixels() + " Min num pixels: " + flob.getMinNumPixels());
	    return flob;
	}
}