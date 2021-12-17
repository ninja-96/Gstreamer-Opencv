#include <iostream>

#include <gst/gst.h>
#include <glib.h>

#include <opencv2/opencv.hpp>


class GstVideoCap {
private:
    int width;
    int height;

    // Create Gstreamer loop, elements, bus, pad and caps
    GMainLoop *loop; 
    GstElement *pipeline, *source, *demuxer, *parser, *decoder, *scale, *convert, *filter, *sink; 
    GstBus *bus; 
    GstPad *out;
    GstCaps *caps; // Need for scale and convert elements to resize and convert color space

public:
    GstVideoCap(std::string filename, int width, int height) {
        this->width = width;
        this->height = height;

        gst_init(NULL, NULL); 
        loop = g_main_loop_new(NULL, FALSE); 

        pipeline = gst_pipeline_new("pipeline"); 

        source = gst_element_factory_make("filesrc", "src"); 
        demuxer = gst_element_factory_make("qtdemux", "demuxer"); 
        parser = gst_element_factory_make("h264parse", "parser"); 
        decoder = gst_element_factory_make("avdec_h264", "decoder"); 

        convert = gst_element_factory_make("videoconvert", "convert");
        scale = gst_element_factory_make("videoscale", "scale");
        filter = gst_element_factory_make("capsfilter", "filter");

        sink = gst_element_factory_make("fakesink", "sink"); 

        caps = gst_caps_new_simple("video/x-raw",
                                   "width", G_TYPE_INT, this->width,
                                   "height", G_TYPE_INT, this->height,
                                   "format", G_TYPE_STRING, "RGBA", NULL);

        if (!pipeline || !source || !demuxer || !parser || !decoder || !convert || !scale || !filter || !sink) { 
            g_printerr("One element could not be created. Exiting.\n");  
        } 

        g_object_set(G_OBJECT(source), "location", filename.c_str(),  NULL); // set location property for filesrc element (filepath)
        g_object_set(G_OBJECT(filter), "caps", caps, NULL); // set caps property for filter to resize image and convert color space
        gst_caps_unref(caps);

        gst_bin_add_many(GST_BIN(pipeline), source, demuxer, parser, decoder, convert, scale, filter, sink, NULL);

        gst_element_link(source, demuxer); 
        gst_element_link_many(parser, decoder, convert, scale, filter, sink, NULL); 
        g_signal_connect(demuxer, "pad-added", G_CALLBACK(qtdemux_parser_link), parser); // link demuxer and h264 parser

        out = gst_element_get_static_pad(sink, "sink");
        gst_pad_add_probe(out, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback)get_probe, this, NULL); // add probe to fakesink to get raw result pipeline execution
    }

    ~GstVideoCap() {
        gst_element_set_state(pipeline, GST_STATE_NULL); 
        gst_object_unref(GST_OBJECT(pipeline)); 
    }

    // start playing pipeline
    void start() {
        gst_element_set_state(pipeline, GST_STATE_PLAYING); 
        g_main_loop_run(loop);
    }

    // custom callback for future user's logic
    void callback(cv::Mat frame) {
        std::cout << "Callback" << std::endl;
        std::cout << frame.size << std::endl;
    }

    // method that help's to link qtdemux and h264parse together
    // this method must be static 
    static void qtdemux_parser_link(GstElement *element, GstPad *pad, GstElement* parser) { 
        GstPad *sinkpad = gst_element_get_static_pad(parser, "sink");
        gst_pad_link(pad, sinkpad); 
        gst_object_unref(sinkpad); 
    } 

    // probe method to process raw data and convert to Opencv Mat image
    // this method must be static 
    static GstPadProbeReturn get_probe(GstPad *pad, GstPadProbeInfo *info, GstVideoCap cap_ptr) {
        // get GstBuffer and map it on GstMapInfo. GstMapInfo has an image as byte array
        GstBuffer *buffer = gst_pad_probe_info_get_buffer(info);
        GstMapInfo map;
        gst_buffer_map(buffer, &map, GST_MAP_READ);

        // Creating Mat image from map.data. Note that you need to know image dimentions.
        cv::Mat frame(cv::Size(cap_ptr.width, cap_ptr.height), CV_8UC4, (char*)map.data, cv::Mat::AUTO_STEP);

        // Image check 
        if (frame.empty()) {
            std::cout << "Empty frame" << std::endl;
        } else {
            cap_ptr.callback(frame);
        }

        gst_buffer_unmap(buffer, &map);
        return GST_PAD_PROBE_OK;
    }
};
