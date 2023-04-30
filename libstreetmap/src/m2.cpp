/* 
 * Copyright 2023 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "globals.h" //contains all global variables defined in m1

/* General Callback functions */
void draw_main_canvas(ezgl::renderer *EZGL);
void drawMap_startup();
void initial_setup(ezgl::application* app, bool new_window);
void act_on_mouse_click(ezgl::application* app, GdkEventButton* event, double x, double y);
void toggle_bg_cbk(GtkWidget* widget, ezgl::application*);
void gtkButton_ML_cbk(GtkWidget *widget, ezgl::application *app);

// toggleable UI revealers
void toggle_help_cbk(GtkWidget *widget, ezgl::application *app);
void toggle_search_cbk(GtkWidget *widget, ezgl::application *app);

// pathfind buttons
void pathfind_next_cbk(GtkWidget* widget, ezgl::application *app);
void pathfind_previous_cbk(GtkWidget* widget, ezgl::application *app);

// toggleable map element display settings
void toggle_pathFind_cbk(GtkWidget* widget, ezgl::application *application);
void toggle_subways_cbk(GtkWidget* widget, ezgl::application *application);
void toggle_featureNames_cbk(GtkWidget* widget, ezgl::application *application);
void toggle_POI_cbk(GtkWidget* widget, ezgl::application *application);
void toggle_POI_names_cbk(GtkWidget* widget, ezgl::application *application);

/* street intersection search callbacks */
void street_text_entry_1(GtkWidget *widget, ezgl::application *application);
void street_text_return_1(GtkWidget *widget, ezgl::application *application);

void street_text_entry_2(GtkWidget *widget, ezgl::application *application);
void street_text_return_2(GtkWidget *widget, ezgl::application *application);

void street_text_entry_3(GtkWidget *widget, ezgl::application *application);
void street_text_return_3(GtkWidget *widget, ezgl::application *application);

void street_text_entry_4(GtkWidget *widget, ezgl::application *application);
void street_text_return_4(GtkWidget *widget, ezgl::application *application);

/* feature search callbacks */
void feature_text_entry_1(GtkWidget *widget, ezgl::application *application);
void feature_text_return_1(GtkWidget *widget, ezgl::application *application);

void feature_text_entry_2(GtkWidget *widget, ezgl::application *application);
void feature_text_return_2(GtkWidget *widget, ezgl::application *application);

/* poi search callbacks */
void poi_text_entry_1(GtkWidget *widget, ezgl::application *application);
void poi_text_return_1(GtkWidget *widget, ezgl::application *application);

void poi_text_entry_2(GtkWidget *widget, ezgl::application *application);
void poi_text_return_2(GtkWidget *widget, ezgl::application *application);

/* street search callbacks */
void single_street_text_entry_1(GtkWidget *widget, ezgl::application *application);
void single_street_text_return_1(GtkWidget *widget, ezgl::application *application);

void single_street_text_entry_2(GtkWidget *widget, ezgl::application *application);
void single_street_text_return_2(GtkWidget *widget, ezgl::application *application);

/* other callbacks */
void window_resize_cbk(GtkWidget *widget, GtkAllocation *alloc, ezgl::application *app);
void clear_highlights_cbk(GtkWidget *widget, ezgl::application *app);

/* Functions for user interactivity */

void clearAllVectorHighlights();
std::string printIntersectInfo(IntersectionIdx intersection);
void setIntersectionHighlights(StreetIdx street1, StreetIdx street2);

//std::vector<std::string> loadPathDirections(std::vector<StreetSegmentIdx> pathSegments);
void loadPathDirections(IntersectionIdx search1, IntersectionIdx search2);

std::string determineDirection (int Ax, int Ay, int Bx, int By, int Cx, int Cy);

// function for displaying the first set of instructions after a return
void displayFirstInstructions (IntersectionIdx search1, IntersectionIdx search2, ezgl::application *application);

int instructionCounter;
std::vector<std::string> pathInstruction;

//global variables to use for drawing the map
const double kClosedPolygonScaleFactor = 0.01;
const double kPointScaleFactor = 0.01;
const double kClosedPolygonTextScaleFactor = 0.04;
const double kPointTextScaleFactor = 0.01;
const double kPointFeatureArea = 10*10;
const double kPoiArea = 4*M_PI;

double worldWidth, worldHeight, screenWidth, screenHeight, scaleFactor;
double worldArea;
double minLat, minLon, maxLat, maxLon;
double scale;
int zoom_level; //bool variable to keep track of what level of zoom we are at
ezgl::rectangle initial_world({0,0},{0,0});
double initialScale;
double area;

std::vector<CourierSubPath> testResultPath;

//BOOLEANS FOR TOGGLEABLE DRAWING
bool pathFindToggle = false;
bool clearBool = false;
bool subwaysToggle = false,
        drawPOIBool = true,
        drawPOINameBool = false,
        drawFeatureNamesBool = false;

// global pathfind intersections, tracked by hitting enter key on intersect search bars on pathfind mode.
IntersectionIdx inter1 = -1, inter2 = -1; //intersection ids for start and end with Intersections
IntersectionIdx inter3 = -1, inter4 = -1; //intersection ids for start and end with mouse clicks
IntersectionIdx inter5 = -1, inter6 = -1; //intersection ids for start and end with Features
IntersectionIdx inter7 = -1, inter8 = -1; //intersection ids for start and end with POIs
IntersectionIdx inter9 = -1, inter10 = -1; //intersection ids for start and end with Streets

IntersectionIdx prevInter1 = -1, prevInter2 = -1; //intersection ids for start and end with Intersections but the previous
IntersectionIdx prevInter5 = -1, prevInter6 = -1; //intersection ids for start and end with Features but the previous
IntersectionIdx prevInter7 = -1, prevInter8 = -1; //intersection ids for start and end with POIs but the previous
IntersectionIdx prevInter9 = -1, prevInter10 = -1; //intersection ids for start and end with Streets but the previous

bool inter1Bool = false, inter2Bool = false; //bool for Intersections path startup
bool inter5Bool = false, inter6Bool = false; //bool for Features path startup
bool inter7Bool = false, inter8Bool = false; //bool for POIs path startup
bool inter9Bool = false, inter10Bool = false; //bool for Streets path startup

int cycleInter = 0;


/* Sets up application to initialize our main window and canvas for ezgl
 */
void drawMap() {
    // sets up the main.ui configuration
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";

    // creates ezgl application
    ezgl::application application(settings);
    drawMap_startup();
    
    application.add_canvas("MainCanvas",draw_main_canvas,initial_world,{220,221,228}); //custom GREY RGB value
    
    application.run(initial_setup, act_on_mouse_click, nullptr, nullptr); // think of these as runtime interrupts from 243
                    //initial_setup, act_on_mouse_click, act_on_mouse_move, act_on_key_press
}

/* 
 * Calculates the startup coordinates for our map as well as the initial scale and area
 */
void drawMap_startup(){
    
//    std::vector<DeliveryInf> testDeliveries = {DeliveryInf(41283, 54262), DeliveryInf(24164, 92899), DeliveryInf(66787, 70120), DeliveryInf(150554, 155285), DeliveryInf(88907, 2754)};
//    std::vector<IntersectionIdx> testDepots = {81319, 142913, 52108};

//    std::vector<DeliveryInf> testDeliveries = {DeliveryInf(175352, 112936), DeliveryInf(50632, 53620), DeliveryInf(152619, 166072), DeliveryInf(28305, 111405), DeliveryInf(10948, 142450), DeliveryInf(48872, 17183), DeliveryInf(27204, 68318), DeliveryInf(61599, 115394), DeliveryInf(75, 15079), DeliveryInf(56272, 183872), DeliveryInf(109883, 33289), DeliveryInf(89979, 174143), DeliveryInf(29415, 147902), DeliveryInf(43091, 33785), DeliveryInf(144547, 25733), DeliveryInf(183579, 63568), DeliveryInf(155334, 45089), DeliveryInf(110877, 33557), DeliveryInf(109040, 38673), DeliveryInf(94366, 3755), DeliveryInf(102862, 22127), DeliveryInf(103831, 165443), DeliveryInf(862, 113022), DeliveryInf(149297, 22564), DeliveryInf(137675, 171695), DeliveryInf(180761, 190746), DeliveryInf(11310, 49229), DeliveryInf(150543, 125445), DeliveryInf(137742, 125639), DeliveryInf(15496, 62346), DeliveryInf(60234, 62528), DeliveryInf(69484, 25179), DeliveryInf(82059, 44660), DeliveryInf(70414, 193857), DeliveryInf(82796, 29026), DeliveryInf(114259, 153591), DeliveryInf(57991, 163444), DeliveryInf(11451, 30358), DeliveryInf(86831, 188073), DeliveryInf(142471, 85973), DeliveryInf(157442, 143138), DeliveryInf(5018, 178697), DeliveryInf(168310, 52430), DeliveryInf(102736, 49720), DeliveryInf(7660, 1858), DeliveryInf(52370, 28866), DeliveryInf(67314, 29767), DeliveryInf(106605, 63275), DeliveryInf(178733, 147886), DeliveryInf(125118, 88823), DeliveryInf(74969, 42680), DeliveryInf(137986, 48715), DeliveryInf(117463, 98315), DeliveryInf(41904, 58317), DeliveryInf(66859, 141477), DeliveryInf(1149, 11085), DeliveryInf(73891, 128355), DeliveryInf(43440, 63788), DeliveryInf(183924, 172763), DeliveryInf(108991, 141958), DeliveryInf(173255, 110264), DeliveryInf(131921, 99840), DeliveryInf(57666, 17742), DeliveryInf(57645, 112461), DeliveryInf(139729, 184597), DeliveryInf(33069, 112741), DeliveryInf(107637, 181070), DeliveryInf(126327, 119523), DeliveryInf(38669, 8191), DeliveryInf(141612, 140766), DeliveryInf(52811, 139312), DeliveryInf(10582, 80074), DeliveryInf(34054, 86734), DeliveryInf(127408, 144381), DeliveryInf(50756, 3238), DeliveryInf(13977, 24763), DeliveryInf(60406, 173270), DeliveryInf(79687, 141382), DeliveryInf(124808, 73000), DeliveryInf(30947, 421), DeliveryInf(18365, 119132), DeliveryInf(154534, 98854), DeliveryInf(28664, 145221), DeliveryInf(82029, 41122), DeliveryInf(63963, 119323), DeliveryInf(76166, 130374), DeliveryInf(170689, 175833), DeliveryInf(24221, 93215), DeliveryInf(57974, 181578), DeliveryInf(107762, 144492), DeliveryInf(120906, 48961), DeliveryInf(37121, 136447), DeliveryInf(97515, 69901), DeliveryInf(164881, 73511), DeliveryInf(141492, 133240), DeliveryInf(175444, 158466), DeliveryInf(51785, 120351), DeliveryInf(90767, 142346), DeliveryInf(12802, 5073), DeliveryInf(103715, 19240), DeliveryInf(6391, 25593), DeliveryInf(66680, 96596), DeliveryInf(52856, 142555), DeliveryInf(86169, 86143), DeliveryInf(152493, 180385), DeliveryInf(38265, 53233), DeliveryInf(126769, 162609), DeliveryInf(175263, 115271), DeliveryInf(110997, 17462), DeliveryInf(126901, 164183), DeliveryInf(17970, 157473), DeliveryInf(15694, 49657), DeliveryInf(9872, 133063), DeliveryInf(172943, 140177), DeliveryInf(10324, 53698), DeliveryInf(127111, 50064), DeliveryInf(150574, 68846), DeliveryInf(69157, 29360), DeliveryInf(30626, 34615), DeliveryInf(30673, 46368), DeliveryInf(129668, 145259), DeliveryInf(28547, 66580), DeliveryInf(65325, 127756), DeliveryInf(190965, 61230), DeliveryInf(45096, 148126), DeliveryInf(108429, 64796), DeliveryInf(172367, 147496), DeliveryInf(1509, 181774), DeliveryInf(131278, 54798), DeliveryInf(164265, 6581), DeliveryInf(181881, 192266), DeliveryInf(171824, 139149), DeliveryInf(28308, 64425), DeliveryInf(22149, 87088), DeliveryInf(18925, 57788), DeliveryInf(89920, 167807), DeliveryInf(53762, 160393), DeliveryInf(172236, 135722), DeliveryInf(67632, 9182), DeliveryInf(184858, 129409), DeliveryInf(160179, 156441), DeliveryInf(170216, 17759), DeliveryInf(104309, 103425), DeliveryInf(29740, 23176), DeliveryInf(93388, 90604), DeliveryInf(169617, 67288), DeliveryInf(115193, 64307), DeliveryInf(177772, 60619), DeliveryInf(156500, 28376), DeliveryInf(57635, 103349), DeliveryInf(162120, 158243), DeliveryInf(142050, 42946), DeliveryInf(146507, 20424), DeliveryInf(161137, 186200), DeliveryInf(46543, 93982), DeliveryInf(107984, 112900), DeliveryInf(50247, 95120), DeliveryInf(13690, 90222), DeliveryInf(146027, 145484), DeliveryInf(86599, 157213), DeliveryInf(119248, 54990), DeliveryInf(132460, 144750), DeliveryInf(130538, 73275), DeliveryInf(76486, 92721), DeliveryInf(173666, 24941), DeliveryInf(37027, 68264), DeliveryInf(10495, 85181), DeliveryInf(165198, 128919), DeliveryInf(102875, 94411), DeliveryInf(83180, 17536), DeliveryInf(75370, 73562), DeliveryInf(40851, 48626), DeliveryInf(45361, 126794), DeliveryInf(13443, 156001), DeliveryInf(164547, 31517)};
//    std::vector<IntersectionIdx> testDepots = {30, 115709, 40719, 46903, 150790, 63728, 117576, 152806, 163358, 39770, 121853, 179374, 14697, 129655, 27882, 113135, 26522, 187706, 16228, 148961};

//    std::vector<DeliveryInf> testDeliveries = {DeliveryInf(1492,121843), DeliveryInf(95436, 120895), DeliveryInf(149593,104798), DeliveryInf(98026, 143272)};
//    std::vector<IntersectionIdx> testDepots = {17611};
//    double testTurnPenalty = 30.0;
//    testResultPath = travelingCourier(testDeliveries, testDepots, testTurnPenalty);  
    
    inter1 = -1, inter6 = -1;
    inter2 = -1, inter7 = -1;
    inter3 = -1, inter8 = -1;
    inter4 = -1, inter9 = -1;
    inter5 = -1, inter10 = -1;

    // Check the first intersection
    minLat = gIntersectVector[0].intersectXYLoc.y;
    minLon = gIntersectVector[0].intersectXYLoc.x;
    maxLat = gIntersectVector[0].intersectXYLoc.y;
    maxLon = gIntersectVector[0].intersectXYLoc.x;
    // Compare with the rest of the intersections
    for(int intersection = 1; intersection < gIntersectVector.size(); intersection++){
        minLat = std::min(minLat,gIntersectVector[intersection].intersectXYLoc.y);
        minLon = std::min(minLon,gIntersectVector[intersection].intersectXYLoc.x);
        maxLat = std::max(maxLat,gIntersectVector[intersection].intersectXYLoc.y);
        maxLon = std::max(maxLon,gIntersectVector[intersection].intersectXYLoc.x);
    }

    // Compare with the rest of the features
    for(int feature_id = 0; feature_id < gFeatureVector.size(); feature_id++){
        minLat = std::min(minLat,gFeatureVector[feature_id].min.y);
        minLon = std::min(minLon,gFeatureVector[feature_id].min.x);
        maxLat = std::max(maxLat,gFeatureVector[feature_id].max.y);
        maxLon = std::max(maxLon,gFeatureVector[feature_id].max.x);
    }

    area = (maxLon - minLon)*(maxLat - minLat);

    initial_world.m_first = {minLon, minLat};
    initial_world.m_second = {maxLon, maxLat};

    initialScale = (maxLon - minLon)*0.1;
    
    cycleInter = 0; // reset intersection mouse click cycler

    inter1 = -1, inter2 = -1; //intersection ids for start and end with Intersections
    inter3 = -1, inter4 = -1; //intersection ids for start and end with mouse clicks
    inter5 = -1, inter6 = -1; //intersection ids for start and end with Features
    inter7 = -1, inter8 = -1; //intersection ids for start and end with POIs
    inter9 = -1, inter10 = -1; //intersection ids for start and end with Streets

    prevInter1 = -1, prevInter2 = -1; //intersection ids for start and end with Intersections but the previous
    prevInter5 = -1, prevInter6 = -1; //intersection ids for start and end with Features but the previous
    prevInter7 = -1, prevInter8 = -1; //intersection ids for start and end with POIs but the previous
    prevInter9 = -1, prevInter10 = -1; //intersection ids for start and end with Streets but the previous
    
    prevInter1 = -1, prevInter2 = -1; //intersection ids for start and end with Intersections
    prevInter5 = -1, prevInter6 = -1; //intersection ids for start and end with Features
    prevInter7 = -1, prevInter8 = -1; //intersection ids for start and end with POIs
    prevInter9 = -1, prevInter10 = -1; //intersection ids for start and end with POIs
    
    inter1Bool = false, inter2Bool = false; //bool for Intersections path startup
    inter5Bool = false, inter6Bool = false; //bool for Features path startup
    inter7Bool = false, inter8Bool = false; //bool for POIs path startup
    inter9Bool = false, inter10Bool = false; //bool for Streets path startup

    
    //testDrawSegs.push_back(99000); // first


//    testDrawSegs.push_back(99000); // first + 1
//    testDrawSegs.push_back(19600); // second + 1
//    testDrawSegs.push_back(19599); // third + 1
//    testDrawSegs.push_back(19598); // fourth + 1
//    testDrawSegs.push_back(19597); // fifth + 1

}

/*void canvas::set_bg_color(ezgl::color){
    m_background_color = {0,0,0};
} */

/* 
 * Callback function that occurs when user pressing the toggle background button
 * Swap the background colour of the Main Canvas
 */
bool toggle_bg = 0; // keeps track if we are in night mode or not
ezgl::color  bg_colour = {237,238,239}; // keeps track of the colour associated with what day/night mode we are in
void toggle_bg_cbk(GtkWidget */*widget*/, ezgl::application *application)
{
    toggle_bg = !toggle_bg; // toggle the bg colour
    if(toggle_bg) bg_colour = {34,41,53}; // toggle bg to dark grey
    else bg_colour = {237,238,239}; // toggle bg light grey
    
    application->refresh_drawing(); // update the drawing with the new background
}

void toggle_pathFind_cbk(GtkWidget* /*widget*/, ezgl::application *app){
    if(!pathFindToggle) pathFindToggle = true;
    else pathFindToggle = false;
    
    GtkWidget *search_entry = (GtkWidget *)app->get_object("intersectionEntry3");
    if(pathFindToggle) gtk_widget_show_all(search_entry);
    else gtk_widget_hide(search_entry);
    
    search_entry = (GtkWidget *)app->get_object("intersectionEntry4");
    if(pathFindToggle) gtk_widget_show_all(search_entry);
    else gtk_widget_hide(search_entry);
    
}

void toggle_subways_cbk(GtkWidget* /*widget*/, ezgl::application *application){
    if(!subwaysToggle) subwaysToggle = true;
    else subwaysToggle = false;

    application->refresh_drawing();
}

void toggle_featureNames_cbk(GtkWidget* /*widget*/, ezgl::application *application){
    if(!drawFeatureNamesBool) drawFeatureNamesBool = true;
    else drawFeatureNamesBool = false;

    application->refresh_drawing();
}

void toggle_POI_cbk(GtkWidget* /*widget*/, ezgl::application *application){
    if(!drawPOIBool) drawPOIBool = true;
    else drawPOIBool = false;

    application->refresh_drawing();
}

void toggle_POI_names_cbk(GtkWidget* /*widget*/, ezgl::application *application){
    if(!drawPOINameBool) drawPOINameBool = true;
    else drawPOINameBool = false;

    application->refresh_drawing();
}

/* 
 * Callback function that occurs when the user selects and presses the button to load in a new map
 * Close the current map
 * Load in the new map
 * Run the calculations necessary at the start of a new map
 * Update the screen to be at the same location of the new map
 */
std::string cur_message; // keeps track of what map is loaded in during run time
void gtkButton_ML_cbk(GtkWidget */*widget*/, ezgl::application *app){
    std::string temp_message = "select map";
    
    // get text in map loader
    GtkComboBoxText *entry = (GtkComboBoxText *)app->get_object("map_loader_dropdown");
    if(gtk_combo_box_text_get_active_text(entry) != nullptr) temp_message = gtk_combo_box_text_get_active_text(entry);
    
    // get map into proper format for calling loadMap
    if(temp_message != "select map" && temp_message != cur_message){ //check if it is an valid input to load a new map
        cur_message = temp_message;

        std::string message_out = "/cad2/ece297s/public/maps/";
        for(int i = 0; i < temp_message.size(); i++){
            temp_message[i] = tolower(temp_message[i]);
            if(temp_message[i] == ' ') {
                temp_message[i] ='-';
            }
            else if(temp_message[i] == ',') {
                temp_message[i] = '_';
                temp_message.erase(temp_message.begin()+(i+1));
            }
        }
        message_out.append(temp_message);
        message_out.append(".streets.bin");
        
        //setup new map
        closeMap();
        loadMap(message_out);
        drawMap_startup();
        
        
        app->change_canvas_world_coordinates("MainCanvas",initial_world); //update the canvas to have the coords of the new world location
        ezgl::renderer *rend = app->get_renderer();
        rend->set_visible_world(initial_world); //update the renderer to have the coords of the new world location
        
        app->refresh_drawing();
    }
}

/*
 * Callback for the revealer that will display or hide the search features on our screen
 */
gboolean toggle_search = 0; // keeps track of whether or not the search features are on the screen or not
void toggle_search_cbk(GtkWidget */*widget*/, ezgl::application *app){
    GtkRevealer *revealer = (GtkRevealer *)app->get_object("RevealSearch");
    toggle_search = !toggle_search;
    gtk_revealer_set_reveal_child(revealer, toggle_search); //reveal based on the toggled search
    
    GtkWidget *pathInstructions = (GtkWidget *)app->get_object("PathGrid");
    if(toggle_search) gtk_widget_show_all(pathInstructions);
    else gtk_widget_hide(pathInstructions);
}

gboolean toggle_help = 0; // keeps track of whether or not the search features are on the screen or not
void toggle_help_cbk(GtkWidget */*widget*/, ezgl::application *app){
    GtkRevealer *revealer = (GtkRevealer *)app->get_object("RevealHelp");
    toggle_help = !toggle_help;
    gtk_revealer_set_reveal_child(revealer, toggle_help); //reveal based on the toggled search
}

// callbacks for previous and next instruction for pathfind
void pathfind_next_cbk(GtkWidget* /*widget*/, ezgl::application *app){

    std::string currentSet;
    currentSet.append("Current Instructions:\n");
    int instructionBuffer = 0;

    
    if(instructionCounter < pathInstruction.size() - 2){
        instructionBuffer = 3;
    }else if(instructionCounter == pathInstruction.size() - 2){
        instructionBuffer = 2;
    }else if(instructionCounter == pathInstruction.size() - 1){
        instructionBuffer = 1;
    }

    // if next thing == arrived, then skip for loop.
    if(instructionCounter < pathInstruction.size()) {
        for (int i = instructionCounter; i < instructionCounter + instructionBuffer; i++) {    // sets of 3 instructions based on the instruction counter.
            currentSet.append(pathInstruction[i]);
        }
    }


    GtkTextBuffer *text_buffer = (GtkTextBuffer *) app->get_object("PathInstructionBuffer");
    //DISPLAY PATH INSTRUCTIONS TO BUFFER
    std::string text_display = currentSet;
    int length = text_display.length();
    gtk_text_buffer_set_text(text_buffer,text_display.c_str(),length);

    if(instructionCounter < pathInstruction.size() - 1){
        instructionCounter++;
    }else{
        instructionCounter = pathInstruction.size() - 1;
    }

    app->refresh_drawing();
}

void pathfind_previous_cbk(GtkWidget* /*widget*/, ezgl::application* app){
    if(instructionCounter > 0){
        instructionCounter --;
    }
    std::string currentSet;
    currentSet.append("Current Instructions:\n");
    int instructionBuffer = 0;

    if(instructionCounter < pathInstruction.size() - 2){
        instructionBuffer = 3;
    }else if(instructionCounter == pathInstruction.size() - 2){
        instructionBuffer = 2;
    }else if(instructionCounter == pathInstruction.size() - 1){
        instructionBuffer = 1;
    }

    if(instructionCounter < pathInstruction.size()) {
        for (int i = instructionCounter; i < instructionCounter + instructionBuffer; i++) {    // sets of 3 instructions based on the instruction counter.
            currentSet.append(pathInstruction[i]);
        }
    }

    GtkTextBuffer *text_buffer = (GtkTextBuffer *) app->get_object("PathInstructionBuffer");
    //DISPLAY PATH INSTRUCTIONS TO BUFFER
    std::string text_display = currentSet;
    int length = text_display.length();
    gtk_text_buffer_set_text(text_buffer,text_display.c_str(),length);

    app->refresh_drawing();
}


/*
 * Callback for when the window is resized to ensure that our overlay widgets remain in their associated spots
 */
void window_resize_cbk(GtkWidget */*widget*/, GtkAllocation *alloc, ezgl::application *app){
    GtkFixed *fixedPlot = (GtkFixed *)app->get_object("FixedPlot");
    
    GtkWidget *panning_tools = (GtkWidget *)app->get_object("PanningTools");
    gtk_fixed_move(fixedPlot,panning_tools,alloc->width - 50,0);
    
    GtkWidget *intersectionInfoPopUp = (GtkWidget *)app->get_object("IntersectionPopUp");
    gtk_fixed_move(fixedPlot,intersectionInfoPopUp,alloc->width - 255.0,300);
    
    GtkWidget *helpButton = (GtkWidget *)app->get_object("HelpRevealer");
    gtk_fixed_move(fixedPlot,helpButton,alloc->width - 145,0);
    
    GtkWidget *helpRevealer = (GtkWidget *)app->get_object("RevealHelp");
    gtk_fixed_move(fixedPlot,helpRevealer,alloc->width - 300,0);
}

void clear_highlights_cbk(GtkWidget */*widget*/, ezgl::application *app){
    //clear all intersections that are highlighted (including path?)
    
    //remove the intersection info box if its being displayed
    GtkWidget *popUp = (GtkWidget *)app->get_object("IntersectionPopUp");
    gtk_widget_hide(popUp);
    
    //reset start and end intersections
    inter1 = -1;
    inter2 = -1;
    prevInter1 = -1;
    prevInter2 = -1;
    inter1Bool = false;
    inter2Bool = false;
    
    inter3 = -1; 
    inter4 = -1;
    cycleInter = 0;
    
    inter5 = -1;
    inter6 = -1;
    prevInter5 = -1;
    prevInter6 = -1;
    inter5Bool = false;
    inter6Bool = false;
    
    inter7 = -1;
    inter8 = -1;
    prevInter7 = -1;
    prevInter8 = -1;
    inter7Bool = false;
    inter8Bool = false;
    
    inter9 = -1;
    inter10 = -1;
    prevInter9 = -1;
    prevInter10 = -1;
    inter9Bool = false;
    inter10Bool = false;
    clearAllVectorHighlights();
    
    app->refresh_drawing(); //update the display
}

void clearAllVectorHighlights(){
    for(int i = 0; i < gIntersectVector.size(); i++){
        gIntersectVector[i].intersectHighlight = false;
    }
    for(int i = 0; i < gSegmentVector.size(); i++){
        gSegmentVector[i].pathHighlight = false;
    }
    for(int i = 0; i < gFeatureVector.size(); i++){
        gFeatureVector[i].featureHighlight = false;
    }
    for(int i = 0; i < gPOIVector.size(); i++){
        gPOIVector[i].poiHighlight = false;
    }
}

/*
 * Callback function for search feature | searching through intersections
 * Called when user enters anything into the search entry
 * Implements auto-suggestion and completion for any user entry
 */
//bool intersectionHasBeenHighlight1 = false, intersectionHasBeenHighlight2 = false; //default intersection has been highlighted off

//int intersectionStreetIDX1 = -1, intersectionStreetIDX2 = -1;
void street_text_entry_1(GtkWidget */*widget*/, ezgl::application *application){
    inter1 = -1;
    
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("intersectionEntry1"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *intersect1Storage  = GTK_LIST_STORE(application->get_object("intersect1ListStore"));

    gtk_list_store_clear(intersect1Storage);
    GtkTreeIter iter;

    std::vector<std::pair<StreetIdx, std::string>> suggestions = getStreetSuggestions(textString);

        for (int i = 0; i < suggestions.size(); i++) {
            std::string testData = suggestions[i].second;
            gtk_list_store_append(intersect1Storage, &iter);
            gtk_list_store_set(intersect1Storage, &iter, 0, testData.c_str(), -1);

        }
}
/*
 * Callback function for search feature | searching through intersections
 * Called when user hits the enter key to submit their query
 * Checks both entries and will change the respective boolean variables to highlight intersections
 */
void street_text_return_1(GtkWidget */*widget*/, ezgl::application *application){
    instructionCounter = 0;

    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("intersectionEntry1"));
    const gchar* text = gtk_entry_get_text(currentEntry);
    GtkEntry* otherEntry = GTK_ENTRY(application->get_object("intersectionEntry2"));
    const gchar* text_other = gtk_entry_get_text(otherEntry);

    std::string street1(text);
    std::string street2(text_other);
    std::vector<StreetIdx> streetVector1 = findStreetIdsFromPartialStreetName(text);
    std::vector<StreetIdx> streetVector2 = findStreetIdsFromPartialStreetName(text_other);
    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName1 = getStreetSuggestions(text);
    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName2 = getStreetSuggestions(text_other);

    std::vector<IntersectionIdx> result;

    if(!pathFindToggle) {
        for (int i = 0; i < streetVector1.size(); i++) {
            for (int j = 0; j < streetVector2.size(); j++) {
                result = findIntersectionsOfTwoStreets(streetVector1[i], streetVector2[j]);
                if (result.size() > 0) {
                    setIntersectionHighlights(streetVector1[i], streetVector2[j]);
                }
            }
        }
    }
    else{
            
        if(autoCompleteName1.size() > 0 && autoCompleteName2.size() > 0){
            result = findIntersectionsOfTwoStreets(autoCompleteName1[0].first, autoCompleteName2[0].first);
            if(result.size() > 0){
                inter1 = result[0]; //take the first intersection
                gIntersectVector[inter1].intersectHighlight = true;
        
            }
        }
        if(inter1 != -1 && inter2 != -1){
            loadPathDirections(inter1, inter2);
        }
        
        displayFirstInstructions(inter1, inter2, application);


    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through intersections
 * Called when user enters anything into the search entry
 * Implements auto-suggestion and completion for any user entry
 */
void street_text_entry_2(GtkWidget */*widget*/, ezgl::application *application){
    inter1 = -1;

    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("intersectionEntry2"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *intersect2Storage  = GTK_LIST_STORE(application->get_object("intersect2ListStore"));

    gtk_list_store_clear(intersect2Storage);
    GtkTreeIter iter;

    std::vector<std::pair<StreetIdx, std::string>> suggestions = getStreetSuggestions(textString);

        for (int i = 0; i < suggestions.size(); i++) {
            std::string testData = suggestions[i].second;
            gtk_list_store_append(intersect2Storage, &iter);
            gtk_list_store_set(intersect2Storage, &iter, 0, testData.c_str(), -1);
        }
}
/*
 * Callback function for search feature | searching through intersections
 * Called when user hits the enter key to submit their query
 * Checks both entries and will change the respective boolean variables to highlight intersections
 */
void street_text_return_2(GtkWidget */*widget*/, ezgl::application *application){
    instructionCounter = 0;

    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("intersectionEntry2"));
    const gchar* text = gtk_entry_get_text(currentEntry);
    GtkEntry* otherEntry = GTK_ENTRY(application->get_object("intersectionEntry1"));
    const gchar* text_other = gtk_entry_get_text(otherEntry);

    std::string street2(text);
    std::string street1(text_other);
    std::vector<StreetIdx> streetVector1 = findStreetIdsFromPartialStreetName(text);
    std::vector<StreetIdx> streetVector2 = findStreetIdsFromPartialStreetName(text_other);
    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName1 = getStreetSuggestions(text);
    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName2 = getStreetSuggestions(text_other);

    std::vector<IntersectionIdx> result;

    if(!pathFindToggle) {
        for (int i = 0; i < streetVector1.size(); i++) {
            for (int j = 0; j < streetVector2.size(); j++) {
                result = findIntersectionsOfTwoStreets(streetVector1[i], streetVector2[j]);
                if (result.size() > 0) {
                    setIntersectionHighlights(streetVector1[i], streetVector2[j]);
                }
            }
        }
    }
    else{
            
        if(autoCompleteName1.size() > 0 && autoCompleteName2.size() > 0){
            result = findIntersectionsOfTwoStreets(autoCompleteName1[0].first, autoCompleteName2[0].first);
            if(result.size() > 0){
                inter1 = result[0];
                gIntersectVector[inter1].intersectHighlight = true;
            }
        }
        if(inter1 != -1 && inter2 != -1){

            std::pair<StreetSegmentIdx, StreetSegmentIdx> thisPair(inter1, inter2);

            //findPathBetweenIntersections(thisPair, 15);
            loadPathDirections(inter1, inter2);

        }
        displayFirstInstructions(inter1, inter2, application);

    }

    application->refresh_drawing();
}

void street_text_entry_3(GtkWidget */*widget*/, ezgl::application *application){

    inter2 = -1;
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("intersectionEntry3"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *intersect3Storage  = GTK_LIST_STORE(application->get_object("intersect3ListStore"));

    gtk_list_store_clear(intersect3Storage);
    GtkTreeIter iter;

    std::vector<std::pair<StreetIdx, std::string>> suggestions = getStreetSuggestions(textString);

    for (int i = 0; i < suggestions.size(); i++) {
        std::string testData = suggestions[i].second;
        gtk_list_store_append(intersect3Storage, &iter);
        gtk_list_store_set(intersect3Storage, &iter, 0, testData.c_str(), -1);

    }
}

void street_text_return_3(GtkWidget */*widget*/, ezgl::application *application){
    instructionCounter = 0;

    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("intersectionEntry3"));
    const gchar* text = gtk_entry_get_text(currentEntry);
    GtkEntry* otherEntry = GTK_ENTRY(application->get_object("intersectionEntry4"));
    const gchar* text_other = gtk_entry_get_text(otherEntry);

    std::string street2(text);
    std::string street1(text_other);
    std::vector<StreetIdx> streetVector1 = findStreetIdsFromPartialStreetName(text);
    std::vector<StreetIdx> streetVector2 = findStreetIdsFromPartialStreetName(text_other);
    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName1 = getStreetSuggestions(text);
    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName2 = getStreetSuggestions(text_other);

    std::vector<IntersectionIdx> result;

    if(!pathFindToggle) {
        for (int i = 0; i < streetVector1.size(); i++) {
            for (int j = 0; j < streetVector2.size(); j++) {
                result = findIntersectionsOfTwoStreets(streetVector1[i], streetVector2[j]);
                if (result.size() > 0) {
                    setIntersectionHighlights(streetVector1[i], streetVector2[j]);
                }
            }
        }
    }else{
            
        
        if(autoCompleteName1.size() > 0 && autoCompleteName2.size() > 0){
            result = findIntersectionsOfTwoStreets(autoCompleteName1[0].first, autoCompleteName2[0].first);
            if(result.size() > 0){
                inter2 = result[0];
                gIntersectVector[inter2].intersectHighlight = true;
            }
        }
        if(inter1 != -1 && inter2 != -1){
            loadPathDirections(inter1, inter2);
            std::pair<StreetSegmentIdx, StreetSegmentIdx> thisPair(inter1, inter2);
            //findPathBetweenIntersections(thisPair, 15);
        }
        displayFirstInstructions(inter1, inter2, application);
    }

    application->refresh_drawing();
}


void street_text_entry_4(GtkWidget * /*widget*/, ezgl::application *application){
    inter2 = -1;

    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("intersectionEntry4"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *intersect4Storage  = GTK_LIST_STORE(application->get_object("intersect4ListStore"));

    gtk_list_store_clear(intersect4Storage);
    GtkTreeIter iter;

    std::vector<std::pair<StreetIdx, std::string>> suggestions = getStreetSuggestions(textString);

    for (int i = 0; i < suggestions.size(); i++) {
        std::string testData = suggestions[i].second;
        gtk_list_store_append(intersect4Storage, &iter);
        gtk_list_store_set(intersect4Storage, &iter, 0, testData.c_str(), -1);

    }
}


void street_text_return_4(GtkWidget * /*widget*/, ezgl::application *application){
    instructionCounter = 0;

    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("intersectionEntry4"));
    const gchar* text = gtk_entry_get_text(currentEntry);
    GtkEntry* otherEntry = GTK_ENTRY(application->get_object("intersectionEntry3"));
    const gchar* text_other = gtk_entry_get_text(otherEntry);

    std::string street2(text);
    std::string street1(text_other);
    std::vector<StreetIdx> streetVector1 = findStreetIdsFromPartialStreetName(text);
    std::vector<StreetIdx> streetVector2 = findStreetIdsFromPartialStreetName(text_other);
    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName1 = getStreetSuggestions(text);
    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName2 = getStreetSuggestions(text_other);

    std::vector<IntersectionIdx> result;

    if(!pathFindToggle) {
        for (int i = 0; i < streetVector1.size(); i++) {
            for (int j = 0; j < streetVector2.size(); j++) {
  //              intersectionHasBeenHighlight2 = true;
                result = findIntersectionsOfTwoStreets(streetVector1[i], streetVector2[j]);
                if (result.size() > 0) {
                    setIntersectionHighlights(streetVector1[i], streetVector2[j]);
                }
            }
        }
    }else{
            
        if(autoCompleteName1.size() > 0 && autoCompleteName2.size() > 0){
            result = findIntersectionsOfTwoStreets(autoCompleteName1[0].first, autoCompleteName2[0].first);
            if(result.size() > 0){
                inter2 = result[0];

                gIntersectVector[inter2].intersectHighlight = true;

            }
        }
        if(inter1 != -1 && inter2 != -1){
            loadPathDirections(inter1, inter2);
            
            std::pair<StreetSegmentIdx, StreetSegmentIdx> thisPair(inter1, inter2);
            //std::vector<std::string> pathInstructions = loadPathDirections(findPathBetweenIntersections(thisPair, 15));
        }
        displayFirstInstructions(inter1, inter2, application);
    }
    application->refresh_drawing();
}

/*
 * Callback function for search feature | searching through features
 * Called when user enters anything into the search entry
 * Implements auto-suggestion and completion for any user entry
 */
bool featureHasBeenHighlight1 = false,featureHasBeenHighlight2 = false; //default feature has been highlighted off

void feature_text_entry_1(GtkWidget */*widget*/, ezgl::application *application){
    featureHasBeenHighlight1 = false;
    inter5 = -1;
    
    for(int i = 0; i < gFeatureVector.size(); i++){     // reset highlighting on edit search bar
        if(!gFeatureVector[i].whichFeatureEntry) gFeatureVector[i].featureHighlight = false;
    }

    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("featureEntry1"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *feature1Storage  = GTK_LIST_STORE(application->get_object("feature1ListStore"));

    gtk_list_store_clear(feature1Storage);
    GtkTreeIter iter;
    
    std::vector<std::string> suggestions = getFeatureSuggestions(textString);

    // get the suggestions list into the search bar suggestions.
    for (int i = 0; i < suggestions.size(); i++) {
        gtk_list_store_append(feature1Storage, &iter);
        gtk_list_store_set(feature1Storage, &iter, 0, suggestions[i].c_str(), -1);

    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through features
 * Called when user hits the enter key to submit their query
 * Checks both entries and will change the respective boolean variables to highlight intersections
 */
void feature_text_return_1(GtkWidget */*widget*/, ezgl::application *application){
    instructionCounter = 0;

    // get the actual inputted text and show its information and location on the map if it exists in the features list
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("featureEntry1"));
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string feature1(text);
    std::vector<std::string> autoCompleteName = getFeatureSuggestions(text);
    
    if(autoCompleteName.size() > 0){
        int i = 0;
        for(; i < gFeatureVector.size(); i++){
            if(autoCompleteName[0] == gFeatureVector[i].featureName){     // account for partial feature name entries
                gFeatureVector[i].whichFeatureEntry = false;
                gFeatureVector[i].featureHighlight = true;
                featureHasBeenHighlight1 = true;
                if(pathFindToggle) {
                    break;
                }
            }
        }
        if(pathFindToggle){
            int inter_id = findClosestIntersection(LatLon(projectLat(gFeatureVector[i].centroid.y),projectLon(gFeatureVector[i].centroid.x))); //find closest intersection to POI
            inter5 = inter_id;
            if(!inter5Bool){
                prevInter5 = inter5;
                inter5Bool = true;
            }
            
            if(inter5 != -1 && inter6 != -1){
                if(inter5Bool){
                    clearBool = true;
                    std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(prevInter5,prevInter6);
                    double turn_penalty = 15;
                    findPathBetweenIntersections(intersect_ids,turn_penalty);                    
                }
                prevInter5 = inter5; //store previous value
                clearBool = false;
                std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(inter5,inter6);
                double turn_penalty = 15;
                std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(intersect_ids,turn_penalty);
            }
            
            if(inter5 != -1 && inter6 != -1){
                loadPathDirections(inter5, inter6);
            }
            displayFirstInstructions(inter5, inter6, application);
        }
    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through features
 * Called when user enters anything into the search entry
 * Implements auto-suggestion and completion for any user entry
 */
void feature_text_entry_2(GtkWidget */*widget*/, ezgl::application *application){
    featureHasBeenHighlight2 = false;
    inter6 = -1;

    for(int i = 0; i < gFeatureVector.size(); i++){     // reset highlighting on edit search bar
        if(gFeatureVector[i].whichFeatureEntry) gFeatureVector[i].featureHighlight = false;
        
    }

    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("featureEntry2"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *feature2Storage  = GTK_LIST_STORE(application->get_object("feature2ListStore"));

    gtk_list_store_clear(feature2Storage);
    GtkTreeIter iter;

    std::vector<std::string> suggestions = getFeatureSuggestions(textString);

    // get the suggestions list into the search bar suggestions.
    for (int i = 0; i < suggestions.size(); i++) {
        gtk_list_store_append(feature2Storage, &iter);
        gtk_list_store_set(feature2Storage, &iter, 0, suggestions[i].c_str(), -1);

    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through features
 * Called when user hits the enter key to submit their query
 * Checks both entries and will change the respective boolean variables to highlight intersections
 */
void feature_text_return_2(GtkWidget */*widget*/, ezgl::application *application){
    instructionCounter = 0;

    // get the actual inputted text and show its information and location on the map if it exists in the features list
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("featureEntry2"));
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string feature2(text);
    std::vector<std::string> autoCompleteName = getFeatureSuggestions(text);
    
    if(autoCompleteName.size() >0){
        int i = 0;
        for(; i < gFeatureVector.size(); i++){
            if(autoCompleteName[0] == gFeatureVector[i].featureName){  // account for partial feature name entries
                gFeatureVector[i].whichFeatureEntry = true;
                gFeatureVector[i].featureHighlight = true;
                featureHasBeenHighlight2 = true;
                if(pathFindToggle){ 
                    break;
                }
            }
        }
        if(pathFindToggle){
//            {
//            projectLat(gFeatureVector[i].centroid.y),
//            projectLon(gFeatureVector[i].centroid.x)
//            }
            int inter_id = findClosestIntersection(LatLon(projectLat(gFeatureVector[i].centroid.y),projectLon(gFeatureVector[i].centroid.x))); //find closest intersection to POI
            inter6 = inter_id;   
            if(!inter6Bool){
                prevInter6 = inter6;
                inter6Bool = true;
            }
            
            if(inter5 != -1 && inter6 != -1){
                if(inter6Bool){
                    clearBool = true;
                    std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(prevInter5,prevInter6);
                    double turn_penalty = 15;
                    findPathBetweenIntersections(intersect_ids,turn_penalty);
                }
                prevInter6 = inter6; //store previous value
                clearBool = false;
                
                std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(inter5,inter6);
                double turn_penalty = 15;
                std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(intersect_ids,turn_penalty);
            }
            //displayFirstInstructions(inter5, inter6, application);
            
            if(inter5 != -1 && inter6 != -1){
                loadPathDirections(inter5, inter6);
            }
            displayFirstInstructions(inter5, inter6, application);
        }
    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through POIs
 * Called when user enters anything into the search entry
 * Implements auto-suggestion and completion for any user entry
 */
void poi_text_entry_1(GtkWidget */*widget*/, ezgl::application *application){
    inter7 = -1;
//    if(inter8 == -1){
//        prevInter7 = -1;
//        inter7Bool = false;
//    }
    
    for(int i = 0; i < gPOIVector.size(); i++){
        if(!gPOIVector[i].whichPOIEntry) gPOIVector[i].poiHighlight = false;
    }
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("poiEntry1"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *poi1Storage  = GTK_LIST_STORE(application->get_object("poi1ListStore"));

    gtk_list_store_clear(poi1Storage);
    GtkTreeIter iter;

    std::vector<std::string> suggestions = getPOISuggestions(textString);

    // get the suggestions list into the search bar suggestions.
    for (int i = 0; i < suggestions.size(); i++) {
        gtk_list_store_append(poi1Storage, &iter);
        gtk_list_store_set(poi1Storage, &iter, 0, suggestions[i].c_str(), -1);

    }
    application->refresh_drawing();
}
bool inter7other, inter8other;
/*
 * Callback function for search feature | searching through POIs
 * Called when user hits the enter key to submit their query
 * Checks both entries and will change the respective boolean variables to highlight intersections
 */
void poi_text_return_1(GtkWidget */*widget*/, ezgl::application *application){
    instructionCounter = 0;

    // get the actual inputted text and show its information and location on the map if it exists in the features list
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("poiEntry1"));
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string poi1(text);
    std::vector<std::string> autoCompleteName = getPOISuggestions(text);
    if(autoCompleteName.size() > 0){
        int i = 0;
        for(; i < gPOIVector.size(); i++) {
            if(autoCompleteName[0] == gPOIVector[i].name){    // display POI from partial entry
                gPOIVector[i].poiHighlight = true;
                gPOIVector[i].whichPOIEntry = false;
                if(pathFindToggle) {
                    break;
                }
            }
        }
        if(pathFindToggle){
            int inter_id = findClosestIntersection(gPOIVector[i].poiLoc); //find closest intersection to POI
            inter7 = inter_id;   
            if(!inter7Bool) {
                prevInter7 = inter7;
                inter7Bool = true;
            }
            
            if(inter7 != -1 && inter8 != -1){
                if(inter7Bool){
                    clearBool = true;
                    std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(prevInter7,prevInter8);
                    double turn_penalty = 15;
                    findPathBetweenIntersections(intersect_ids,turn_penalty);     
                }
                prevInter7 = inter7; //store previous value
                clearBool = false;
                
                std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(inter7,inter8);
                double turn_penalty = 15;
                std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(intersect_ids,turn_penalty);
            }
            if(inter7 != -1 && inter8 != -1){
                loadPathDirections(inter7, inter8);
            }
            displayFirstInstructions(inter7, inter8, application);
        }
    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through POIs
 * Called when user enters anything into the search entry
 * Implements auto-suggestion and completion for any user entry
 */
void poi_text_entry_2(GtkWidget */*widget*/, ezgl::application *application){
    inter8 = -1;
//    if(inter7 == -1) {
//        prevInter8 = -1; 
//        inter8Bool = false;
//    }
    for(int i = 0; i < gPOIVector.size(); i++){
        if(gPOIVector[i].whichPOIEntry) gPOIVector[i].poiHighlight = false;
    }
    
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("poiEntry2"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *poi2Storage  = GTK_LIST_STORE(application->get_object("poi2ListStore"));

    gtk_list_store_clear(poi2Storage);
    GtkTreeIter iter;

    std::vector<std::string> suggestions = getPOISuggestions(textString);

    // get the suggestions list into the search bar suggestions.
    for (int i = 0; i < suggestions.size(); i++) {
        gtk_list_store_append(poi2Storage, &iter);
        gtk_list_store_set(poi2Storage, &iter, 0, suggestions[i].c_str(), -1);

    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through POIs
 * Called when user hits the enter key to submit their query
 * Checks both entries and will change the respective boolean variables to highlight intersections
 */
void poi_text_return_2(GtkWidget */*widget*/, ezgl::application *application){
    instructionCounter = 0;

    // get the actual inputted text and show its information and location on the map if it exists in the features list
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("poiEntry2"));
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string poi2(text);
    std::vector<std::string> autoCompleteName = getPOISuggestions(text);
    
    if(autoCompleteName.size() > 0){
        int i = 0;
        for(; i < gPOIVector.size(); i++) {
            if(autoCompleteName[0] == gPOIVector[i].name){    // display POI from partial entry
                gPOIVector[i].poiHighlight = true;
                gPOIVector[i].whichPOIEntry = true;
                if(pathFindToggle){ 
                    break;
                }
            }
        }
        if(pathFindToggle){
            //bool inter6Check;
            int inter_id = findClosestIntersection(gPOIVector[i].poiLoc); //find closest intersection to POI
            inter8 = inter_id;
            if(!inter8Bool){ 
                prevInter8 = inter8;
                inter8Bool = true;             
            }
            
            if(inter7 != -1 && inter8 != -1){
                if(inter8Bool){
                    clearBool = true;
                    std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(prevInter7,prevInter8);
                    double turn_penalty = 15;
                    findPathBetweenIntersections(intersect_ids,turn_penalty);
                }
                prevInter8 = inter8; //store previous value
                clearBool = false;
                std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(inter7,inter8);
                double turn_penalty = 15;
                std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(intersect_ids,turn_penalty);
            }
            if(inter7 != -1 && inter8 != -1){
                loadPathDirections(inter7, inter8);
            }
            displayFirstInstructions(inter7, inter8, application);
        }
    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through Streets
 * Called when user enters anything into the search entry
 * Implements auto-suggestion and completion for any user entry
 */
void single_street_text_entry_1(GtkWidget */*widget*/, ezgl::application *application){
    inter9 = -1;
    for(int i = 0; i < gStreetsVector.size(); i++){     // reset highlighting on edit search bar
        if(!gStreetsVector[i].whichStreetEntry) gStreetsVector[i].streetHighlight = false;
    }
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("streetEntry1"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *intersect1Storage  = GTK_LIST_STORE(application->get_object("street1ListStore"));

    gtk_list_store_clear(intersect1Storage);
    GtkTreeIter iter;

    std::vector<std::pair<StreetIdx, std::string>> suggestions = getStreetSuggestions(textString);

    for (int i = 0; i < suggestions.size(); i++) {
        std::string testData = suggestions[i].second;
        gtk_list_store_append(intersect1Storage, &iter);
        gtk_list_store_set(intersect1Storage, &iter, 0, testData.c_str(), -1);
    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through Streets
 * Called when user hits the enter key to submit their query
 * Checks both entries and will change the respective boolean variables to highlight intersections
 */
void single_street_text_return_1(GtkWidget */*widget*/, ezgl::application *application){
    instructionCounter = 0;

    // get the actual inputted text and show its information and location on the map if it exists in the features list
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("streetEntry1"));
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string singleStreet1(text);

    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName = getStreetSuggestions(text);
    if(autoCompleteName.size() > 0){
        int i = 0;
        for(; i < gStreetsVector.size(); i++) {
            if(autoCompleteName[0].second == gStreetsVector[i].streetName){     // text entry doesnt match a database name but highlights the first suggestion on partial name
                gStreetsVector[autoCompleteName[0].first].streetHighlight = true;
                gStreetsVector[autoCompleteName[0].first].whichStreetEntry = false;
                if(pathFindToggle) {
                    break;
                }
            }
        }
        if(pathFindToggle){
            int numOfSegs = gStreetsVector[i].segsOfStreet.size();
            inter9 = gIntersectVector[gSegmentVector[gStreetsVector[i].segsOfStreet[numOfSegs/2]].from].interIndex;
            if(!inter9Bool){
                prevInter9 = inter9;
                inter9Bool = true;
            }
            if(inter9 != -1 && inter10 != -1){
                if(inter9Bool){
                    clearBool = true;
                    std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(prevInter9,prevInter10);
                    double turn_penalty = 15;
                    findPathBetweenIntersections(intersect_ids,turn_penalty);
                }
                prevInter9 = inter9; //store previous value
                clearBool = false;
                
                std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(inter9,inter10);
                double turn_penalty = 15;
                std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(intersect_ids,turn_penalty);
            }
            if(inter9 != -1 && inter10 != -1){
                loadPathDirections(inter9, inter10);
            }
            displayFirstInstructions(inter9, inter10, application);
        }
    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through Streets
 * Called when user enters anything into the search entry
 * Implements auto-suggestion and completion for any user entry
 */
void single_street_text_entry_2(GtkWidget */*widget*/, ezgl::application *application){
    inter10 = -1;
    for(int i = 0; i < gStreetsVector.size(); i++){     // reset highlighting on edit search bar
        if(gStreetsVector[i].whichStreetEntry) gStreetsVector[i].streetHighlight = false;
    }

    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("streetEntry2"));  // current text entry i.e. prefixes
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string textString(text);

    GtkListStore *intersect1Storage  = GTK_LIST_STORE(application->get_object("street2ListStore"));

    gtk_list_store_clear(intersect1Storage);
    GtkTreeIter iter;

    std::vector<std::pair<StreetIdx, std::string>> suggestions = getStreetSuggestions(textString);

    for (int i = 0; i < suggestions.size(); i++) {
        std::string testData = suggestions[i].second;
        gtk_list_store_append(intersect1Storage, &iter);
        gtk_list_store_set(intersect1Storage, &iter, 0, testData.c_str(), -1);

    }
    application->refresh_drawing();
}
/*
 * Callback function for search feature | searching through Streets
 * Called when user hits the enter key to submit their query
 * Checks both entries and will change the respective boolean variables to highlight intersections
 */
void single_street_text_return_2(GtkWidget */*widget*/, ezgl::application *application){
    instructionCounter = 0;

    // get the actual inputted text and show its information and location on the map if it exists in the features list
    GtkEntry* currentEntry = GTK_ENTRY(application->get_object("streetEntry2"));
    const gchar* text = gtk_entry_get_text(currentEntry);

    std::string singleStreet2(text);
    
    std::vector<std::pair<StreetIdx, std::string>> autoCompleteName = getStreetSuggestions(text);
    if(autoCompleteName.size() > 0){
        int i = 0;
        for(; i < gStreetsVector.size(); i++) {
            if(autoCompleteName[0].second == gStreetsVector[i].streetName){     // text entry doesnt match a database name but highlights the first suggestion on partial name
                gStreetsVector[autoCompleteName[0].first].streetHighlight = true;
                gStreetsVector[autoCompleteName[0].first].whichStreetEntry = true;
                if(pathFindToggle) {
                    break;
                }
            }
        }
        if(pathFindToggle){
            int numOfSegs = gStreetsVector[i].segsOfStreet.size();
            inter10 = gIntersectVector[gSegmentVector[gStreetsVector[i].segsOfStreet[numOfSegs/2]].from].interIndex;
            if(!inter10Bool) {   
                prevInter10 = inter10; //get previous inter10 the first time around
                inter10Bool = true;
            }
            if(inter9 != -1 && inter10 != -1){
                if(inter10Bool){
                    clearBool = true;
                    std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(prevInter9,prevInter10);
                    double turn_penalty = 15;
                    findPathBetweenIntersections(intersect_ids,turn_penalty);
                }
                prevInter10 = inter10; //store previous value
                clearBool = false;
                
                std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(inter9,inter10);
                double turn_penalty = 15;
                std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(intersect_ids,turn_penalty);
            }

            if(inter9 != -1 && inter10 != -1){
                loadPathDirections(inter9, inter10);
            }
            displayFirstInstructions(inter9, inter10, application);
        }
    }
    application->refresh_drawing();
}

/*
 * Callback function for when a user clicks on the screen
 * Detects if the user is clicking on an intersection
 * If so, the pop up window is activated and the intersection's info is displayed
 */
bool mouse_click_assign_inter = false;
void act_on_mouse_click(ezgl::application* app, GdkEventButton* /*event*/, double x, double y){
    //find closest intersection to a mouse click
    LatLon clickPos = LatLon(projectLat(y),projectLon(x));
    int inter_id = findClosestIntersection(clickPos);
    
    std::cout << inter_id << "\n";
    
    //check if click was close enough to the intersection
    /* Absolute value of (click position - intersection position) > some length value
     */
    double height = 10;
    double width = height;

    GtkWidget *popUp = (GtkWidget *)app->get_object("IntersectionPopUp");
    
    if(fabs(projectX(clickPos.longitude()) - gIntersectVector[inter_id].intersectXYLoc.x) < width &&
       fabs(projectY(clickPos.latitude()) - gIntersectVector[inter_id].intersectXYLoc.y) < height){
        gtk_widget_hide(popUp);

        if(!gIntersectVector[inter_id].intersectHighlight){
            mouse_click_assign_inter = !mouse_click_assign_inter;
//            if(cycleInter >= 2){
//                inter3 = inter4;
//                inter4 = inter_id;
//            }
//            else
            if(mouse_click_assign_inter) {
                inter3 = inter_id;
                cycleInter++;
            }
            else {
                inter4 = inter_id;
                cycleInter++;
            }
            
            if(inter3 != -1 && inter4 != -1 && pathFindToggle){
                instructionCounter = 0;
                std::pair<IntersectionIdx, IntersectionIdx> intersect_ids = std::make_pair(inter3,inter4);
                double turn_penalty = 15;
                std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(intersect_ids,turn_penalty);
                // loadPathDirections(path);
                
                
                loadPathDirections(inter3, inter4);
                                displayFirstInstructions(inter3, inter4, app);

                inter3 = -1;
                inter4 = -1;
                cycleInter = 0;
            }
            if(pathFindToggle) {

            }
            gIntersectVector[inter_id].intersectHighlight = true; //update intersection highlight
            
            
            //TEXT BUFFER BOX TO PRINT OUT INTERSECTION INFO
            GtkTextBuffer *text_buffer = (GtkTextBuffer *) app->get_object("IntersectionInfo");
    
            //DISPLAY INFO ABOUT INTERSECTION TO BUFFER
            std::string text_display = printIntersectInfo(inter_id);
            int length = text_display.length();
            gtk_text_buffer_set_text(text_buffer,text_display.c_str(),length);

            gtk_widget_show_all(popUp); //display Text Buffer Box
        }
        else{  // unhighlight already highlighted intersections
            gIntersectVector[inter_id].intersectHighlight = false; //update intersection highlight
            
            //check if we remove a highlight that was the start or end
            if(inter_id == inter3) { // start removed
                cycleInter -= 2;
            }
            else if(inter_id == inter4){
                cycleInter--;
            }
        }
    }

    app->refresh_drawing(); //force redraw call to update the selection; considering drawing over top of intersection instead of refreshing the entire draw
}
/*
 * Sets up the canvas at the start of the run time
 * Removes all arrow widgets (unnecessary)
 * Removes proceed button
 * Sets up the Pop Up window for intersection pop ups to be dark grey colour
 * Sets up all callback functions to the appropriate signals that generate them (see callback functions above)
 */
void initial_setup(ezgl::application* app, bool /*new_window*/){
    
    // Get rid of unnecessary panning buttons
    GtkWidget *button_remove = (GtkWidget *)app->get_object("UpButton");
    gtk_widget_destroy(button_remove);
    button_remove = (GtkWidget *)app->get_object("DownButton");
    gtk_widget_destroy(button_remove);
    button_remove = (GtkWidget *)app->get_object("RightButton");
    gtk_widget_destroy(button_remove);
    button_remove = (GtkWidget *)app->get_object("LeftButton");
    gtk_widget_destroy(button_remove);

    // Get rid of proceed button
    button_remove = (GtkWidget *)app->get_object("ProceedButton");
    gtk_widget_destroy(button_remove);
    
    GObject *main_canvas = app->get_object("MainCanvas");
    g_signal_connect(
        main_canvas,
        "size-allocate",
        G_CALLBACK(window_resize_cbk),
        app
    );
    
    GObject *search_revealer = app->get_object("SearchRevealer");
    g_signal_connect(
            search_revealer,
            "clicked",
            G_CALLBACK(toggle_search_cbk),
            app
    );
    
    GObject *help_revealer = app->get_object("HelpRevealer");
    g_signal_connect(
            help_revealer,
            "clicked",
            G_CALLBACK(toggle_help_cbk),
            app
    );

    GObject *next_instruction = app->get_object("PathNext");
    g_signal_connect(
            next_instruction,
            "clicked",
            G_CALLBACK(pathfind_next_cbk),
            app
    );

    GObject *previous_instruction = app->get_object("PathPrevious");
    g_signal_connect(
            previous_instruction,
            "clicked",
            G_CALLBACK(pathfind_previous_cbk),
            app
    );
    
    GObject *clear_button = app->get_object("IntersectionClear");
    g_signal_connect(
            clear_button,
            "clicked",
            G_CALLBACK(clear_highlights_cbk),
            app
    );
    
    //gtk_widget_show_all(main_overlay);
    //gtk_widget_hide(main_overlay);
    // Get a pointer to the UI widget, using the ID string you set for it in glade
    GObject *toggle_background = app->get_object("toggle_bg");
    g_signal_connect(
        toggle_background, // pointer to the UI widget
        "clicked", // Signal name (from the widget documentation) e.g. "clicked"
        G_CALLBACK(toggle_bg_cbk), // name of callback function (you write this function)
        app // pointer to (arbitrary) data to pass to callback my_func or NULL for no data
    );

    // signal connect for the pathfinder toggle button
    GObject *toggle_pathFind = app->get_object("toggle_pathFind");
    g_signal_connect(
            toggle_pathFind, // pointer to the UI widget
            "toggled", // Signal state of toggle being changed
            G_CALLBACK(toggle_pathFind_cbk),
            app );

    // signal connect for the display subways toggle button
    GObject *toggle_subways = app->get_object("toggle_subways");
    g_signal_connect(
            toggle_subways, // pointer to the UI widget
            "toggled", // Signal state of toggle being changed
            G_CALLBACK(toggle_subways_cbk),
            app );

    // signal connect for the display feature names toggle button
    GObject *toggle_featureNames = app->get_object("toggle_featureNames");
    g_signal_connect(
            toggle_featureNames, // pointer to the UI widget
            "toggled", // Signal state of toggle being changed
            G_CALLBACK(toggle_featureNames_cbk),
            app );

    GObject *toggle_POI = app->get_object("toggle_POI");
    g_signal_connect(
            toggle_POI, // pointer to the UI widget
            "toggled", // Signal state of toggle being changed
            G_CALLBACK(toggle_POI_cbk),
            app );

    GObject *toggle_POI_names = app->get_object("toggle_POI_names");
    g_signal_connect(
            toggle_POI_names, // pointer to the UI widget
            "toggled", // Signal state of toggle being changed
            G_CALLBACK(toggle_POI_names_cbk),
            app );

    // MAP LOADER GTK SETUP
    GObject *gtkButton_ML = app->get_object("map_loader_button");
    g_signal_connect(
        gtkButton_ML,
        "clicked",
        G_CALLBACK(gtkButton_ML_cbk),
        app
    );

    GObject *intersectionEntry2 = app->get_object("intersectionEntry2");
    g_signal_connect(intersectionEntry2, "changed", G_CALLBACK(street_text_entry_2), app);   // call back happens always here.
    g_signal_connect(intersectionEntry2, "activate", G_CALLBACK(street_text_return_2), app);

    GObject *intersectionEntry1 = app->get_object("intersectionEntry1");
    g_signal_connect(intersectionEntry1, "changed", G_CALLBACK(street_text_entry_1), app);   // call back happens always here.
    g_signal_connect(intersectionEntry1, "activate", G_CALLBACK(street_text_return_1), app);

    GObject *intersectionEntry3 = app->get_object("intersectionEntry3");
    g_signal_connect(intersectionEntry3, "changed", G_CALLBACK(street_text_entry_3), app);   // call back happens always here.
    g_signal_connect(intersectionEntry3, "activate", G_CALLBACK(street_text_return_3), app);

    GObject *intersectionEntry4 = app->get_object("intersectionEntry4");
    g_signal_connect(intersectionEntry4, "changed", G_CALLBACK(street_text_entry_4), app);   // call back happens always here.
    g_signal_connect(intersectionEntry4, "activate", G_CALLBACK(street_text_return_4), app);

    GObject *featureEntry1 = app->get_object("featureEntry1");
    g_signal_connect(featureEntry1, "changed", G_CALLBACK(feature_text_entry_1), app);   // call back happens always here.
    g_signal_connect(featureEntry1, "activate", G_CALLBACK(feature_text_return_1), app);

    GObject *featureEntry2 = app->get_object("featureEntry2");
    g_signal_connect(featureEntry2, "changed", G_CALLBACK(feature_text_entry_2), app);   // call back happens always here.
    g_signal_connect(featureEntry2, "activate", G_CALLBACK(feature_text_return_2), app);

    GObject *POIEntry1 = app->get_object("poiEntry1");
    g_signal_connect(POIEntry1, "changed", G_CALLBACK(poi_text_entry_1), app);   // call back happens always here.
    g_signal_connect(POIEntry1, "activate", G_CALLBACK(poi_text_return_1), app);

    GObject *POIEntry2 = app->get_object("poiEntry2");
    g_signal_connect(POIEntry2, "changed", G_CALLBACK(poi_text_entry_2), app);   // call back happens always here.
    g_signal_connect(POIEntry2, "activate", G_CALLBACK(poi_text_return_2), app);

    GObject *SingleStreetEntry1 = app->get_object("streetEntry1");
    g_signal_connect(SingleStreetEntry1, "changed", G_CALLBACK(single_street_text_entry_1), app);   // call back happens always here.
    g_signal_connect(SingleStreetEntry1, "activate", G_CALLBACK(single_street_text_return_1), app);

    GObject *SingleStreetEntry2 = app->get_object("streetEntry2");
    g_signal_connect(SingleStreetEntry2, "changed", G_CALLBACK(single_street_text_entry_2), app);   // call back happens always here.
    g_signal_connect(SingleStreetEntry2, "activate", G_CALLBACK(single_street_text_return_2), app);
}

/*
 * Function for redrawing the map
 */
void draw_main_canvas(ezgl::renderer *EZGL){
    
    auto start = std::chrono::system_clock::now(); 
    
    worldWidth = EZGL->get_visible_world().width();
    worldHeight = EZGL->get_visible_world().height();
    screenWidth = EZGL->get_visible_screen().width();
    screenHeight = EZGL->get_visible_screen().height();
    worldArea = EZGL->get_visible_world().area();
    scaleFactor = worldWidth/screenWidth;
    
    //set scale length
    double scale_length = 0.1 * screenWidth; //10% of the screen

    //calculate scale value and display
    scale = scaleFactor*scale_length;
    zoom_level = round(log(scale/initialScale )/(-0.511));
    //std::cout << "Zoom: " << zoom_level << "\n";

    EZGL->set_coordinate_system(ezgl::WORLD);

    //draw background
    EZGL->set_color(bg_colour);
    EZGL->fill_rectangle({EZGL->get_visible_world().bottom_left()},{EZGL->get_visible_world().top_right()});
    
    //draw map background
    ezgl::color map_bg_colour = ezgl::color((bg_colour.red - 15),(bg_colour.green - 15),(bg_colour.blue - 15));
    EZGL->set_color(map_bg_colour);
    EZGL->fill_rectangle({minLon,minLat},{maxLon,maxLat});
    
    auto test_start = std::chrono::system_clock::now();
    // Draw all closed polygon features
    drawClosedFeatures(EZGL);
    auto test_end = std::chrono::system_clock::now();
    std::chrono::duration<double> test_diff = test_end - test_start;
    //std::cout << "Draw Closed Features: " << test_diff.count() << "\n";
    
    test_start = std::chrono::system_clock::now();
    // Draw all line features
    drawLineFeatures(EZGL);
    test_end = std::chrono::system_clock::now();
    test_diff = test_end - test_start;
    //std::cout << "Draw Line Features: " << test_diff.count() << "\n";
    
    test_start = std::chrono::system_clock::now();
    // Draw all single point features as 10x10 boxes
    drawPointFeatures(EZGL);
    test_end = std::chrono::system_clock::now();
    test_diff = test_end - test_start;
    //std::cout << "Draw Point Features: " << test_diff.count() << "\n";

    test_start = std::chrono::system_clock::now();
    //draw all street segments
    drawStreets(EZGL);
    test_end = std::chrono::system_clock::now();
    test_diff = test_end - test_start;
    //std::cout << "Draw Streets: " << test_diff.count() << "\n";
    
    test_start = std::chrono::system_clock::now();

    //std::vector<StreetSegmentIdx> testPath = findPathBetweenIntersections(std::make_pair(119350, 98286), 0);
    drawStreetPath(EZGL);

    //draw all subway stations

    if(subwaysToggle) drawSubWayStations(EZGL);

    test_end = std::chrono::system_clock::now();
    test_diff = test_end - test_start;
    //std::cout << "Draw Subways: " << test_diff.count() << "\n";

    test_start = std::chrono::system_clock::now();
    //draw all intersect points that are to be highlighted
    //if(intersectionHasBeenHighlight1 && intersectionHasBeenHighlight2) 
    drawIntersectHighlights(EZGL);
    test_end = std::chrono::system_clock::now();
    test_diff = test_end - test_start;
    //std::cout << "Draw Intersections: " << test_diff.count() << "\n";

    test_start = std::chrono::system_clock::now();
    //draw all feature points that are to be highlighted
    if(featureHasBeenHighlight1 || featureHasBeenHighlight2) drawFeatureHighlights(EZGL);    
    test_end = std::chrono::system_clock::now();
    test_diff = test_end - test_start;
    //std::cout << "Draw Feature Highlights: " << test_diff.count() << "\n";
    
    test_start = std::chrono::system_clock::now();
    //draw all POIs
    if(drawPOIBool) drawPOIs(EZGL);
    test_end = std::chrono::system_clock::now();
    test_diff = test_end - test_start;
    //std::cout << "Draw POIs: " << test_diff.count() << "\n";
    
    if(drawFeatureNamesBool){
        test_start = std::chrono::system_clock::now();
        // Draw the name for closed features
        displayClosedFeatureName(EZGL);
        test_end = std::chrono::system_clock::now();
        test_diff = test_end - test_start;
        //std::cout << "Draw Closed Feature Names: " << test_diff.count() << "\n";

        // Draw the name for the line features
        test_start = std::chrono::system_clock::now();
        displayLineFeatureName(EZGL);
        test_end = std::chrono::system_clock::now();
        test_diff = test_end - test_start;
        //std::cout << "Draw Line Feature Names: " << test_diff.count() << "\n";

        // Draw the name for single point features
        test_start = std::chrono::system_clock::now();
        displayPointFeatureName(EZGL);
        test_end = std::chrono::system_clock::now();
        test_diff = test_end - test_start;
        //std::cout << "Draw Point Feature Names: " << test_diff.count() << "\n";
    }
    test_start = std::chrono::system_clock::now();
    // Draw the name for POIs
    if(drawPOINameBool) displayPOIName(EZGL);
    test_end = std::chrono::system_clock::now();
    test_diff = test_end - test_start;
    //std::cout << "Draw POI Names: " << test_diff.count() << "\n"; 
    
    //draw start and end highlights
//    EZGL->draw_surface(find_poi_1, gIntersectVector[testResultPath[0].start_intersection].intersectXYLoc, 0.18);
//    //EZGL->draw_surface(find_poi_2, gIntersectVector[testResultPath[testResultPath.size() - 1].end_intersection].intersectXYLoc, 0.18);
//    
//    for(int i = 1; i < testResultPath.size() - 1; i++){
//        EZGL->draw_surface(intersection_highlight, gIntersectVector[testResultPath[i].start_intersection].intersectXYLoc, 0.18);
//        EZGL->draw_surface(intersection_highlight, gIntersectVector[testResultPath[i+1].start_intersection].intersectXYLoc, 0.18);
//    }
    
    /*CONVER INTO SCREEN AND DISPLAY HERE*/
    EZGL->set_coordinate_system(ezgl::SCREEN);

    //draw box for scale
    EZGL->set_color(ezgl::WHITE);
    EZGL->fill_rectangle({screenWidth - scale_length, screenHeight - 10},{screenWidth, screenHeight - 30});

    //draw scale line base
    EZGL->set_line_width(2);
    EZGL->set_color(ezgl::BLACK);
    EZGL->draw_line({screenWidth - scale_length, screenHeight - 10}, {screenWidth, screenHeight - 10});

    //draw scale onto box
    ezgl::font_slant slant {CAIRO_FONT_SLANT_NORMAL}; //need to reference ezgl and not ezgl::renderer for this to work
    ezgl::font_weight weight {CAIRO_FONT_WEIGHT_NORMAL};
    EZGL->set_text_rotation(0);
    EZGL->format_font("OpenSans",slant,weight);
    EZGL->set_font_size(12);
    EZGL->set_color(ezgl::BLACK);

    std::string scale_output = std::to_string(scale);
    std::size_t pos = scale_output.find(".");
    scale_output = scale_output.substr(0,pos+3);
    scale_output.append(" m");
    EZGL->draw_text({screenWidth - scale_length/2,screenHeight - 20},scale_output);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "redraw time: " << diff.count() << " | ";
    double FPS = 1/diff.count();
    std::cout << FPS << " fps\n";
    
}

/* function that returns the string corresponding to information of intersections that have been clicked on. this is in full
 * display format that is ready to be put into the visualized text box on the UI */
std::string printIntersectInfo(IntersectionIdx intersection) {
    std::vector<StreetSegmentIdx> connectedSegments = gIntersectVector[intersection].connectedStreetSegments;
    std::vector<std::string> connectedStreetNames; // connected streets to that intersection
    std::vector<double> speedLimits; // for each street segment
    std::string result;

    for (int i = 0; i < connectedSegments.size(); i++) {
        connectedStreetNames.push_back(gStreetsVector[gSegmentVector[connectedSegments[i]].myStreet].streetName);
    }

    std::sort(connectedStreetNames.begin(), connectedStreetNames.end());
    connectedStreetNames.erase(std::unique(connectedStreetNames.begin(), connectedStreetNames.end()), connectedStreetNames.end());

    result.append("Intersection Location: \n(");
    std::string intersectionLongitude = std::to_string(round(100*gIntersectVector[intersection].intersectLoc.latitude())/100);
    std::size_t pos1 = intersectionLongitude.find(".");
    intersectionLongitude = intersectionLongitude.substr(0,pos1+3);
    result.append(intersectionLongitude);
    result.append(", ");
    std::string intersectionLatitude = std::to_string(round(100*gIntersectVector[intersection].intersectLoc.latitude())/100);
    std::size_t pos2 = intersectionLatitude.find(".");
    intersectionLatitude = intersectionLatitude.substr(0,pos2+3);
    result.append(intersectionLatitude);
    
    int count = 0;
    
    result.append(")\n");
    result.append("Connected Streets:");
    for (int i = 0; i < connectedStreetNames.size(); i++) {
        count++;
        std::string countString = std::to_string(count);
        result.append("\n");
        result.append(countString);
        result.append(". \" ");//~~");
        result.append(connectedStreetNames[i]);
        result.append(" \"\n| Speed Limit = ");
        result.append(std::to_string((int)(round((gSegmentVector[connectedSegments[i]].segSpeedLimit)*3.6))));
        result.append("|\n| One Way = ");
        if(gSegmentVector[connectedSegments[i]].oneWay) result.append("Yes");
        else result.append("No");
        result.append(" |");
        result.append("\nid: ");
        result.append(std::to_string(connectedSegments[i]));
    }

    return result;
}

// function to draw intersection between 2 entered streets
void setIntersectionHighlights (StreetIdx street1, StreetIdx street2){
    std::vector<IntersectionIdx> inputIntersections = findIntersectionsOfTwoStreets(street1, street2);
    for(int i = 0; i < inputIntersections.size(); i++){
        IntersectionIdx interIDX = inputIntersections[i];
        gIntersectVector[interIDX].intersectHighlight = true;
    }
    // gIntersectVector[30666].intersectHighlight = true;

}

void loadPathDirections(IntersectionIdx search1, IntersectionIdx search2){
    pathInstruction.clear();
    //std::vector<std::string>().swap(pathInstruction);  // clear vector before using it.
    //std::vector<std::string> pathInstructions;

    std::pair<StreetSegmentIdx, StreetSegmentIdx> thisPair(search1, search2);   // changing between inter1-inter10.
    std::vector<StreetSegmentIdx> pathSegments = findPathBetweenIntersections(thisPair, 15);

    std::string estimatedTime;
    estimatedTime.append("Estimated Time: ");
    estimatedTime.append( std::to_string((int)(computePathTravelTime(pathSegments, 15)) / 60));
    estimatedTime.append(" min");
    estimatedTime.append("\n");

    pathInstruction.push_back(estimatedTime);
    std::string direction;
    std::string resultSingle;

    // for one segment paths. //
    IntersectionIdx trueTo = gSegmentVector[pathSegments[0]].to;
    
    if(gIntersectVector[trueTo].intersectXYLoc == gIntersectVector[search1].intersectXYLoc){
        trueTo = gSegmentVector[pathSegments[0]].from;
    }
    
    double deltaX = gIntersectVector[trueTo].intersectXYLoc.x - gIntersectVector[search1].intersectXYLoc.x;
    double deltaY = (gIntersectVector[trueTo].intersectXYLoc.y - gIntersectVector[search1].intersectXYLoc.y);
    double angle = atan(deltaY/deltaX);
    
    double PI = 3.14159;
        
    // get the initial direction
    if( (angle <= -PI/4 && angle > -PI/2 && deltaX >= 0 && deltaY < 0) || (angle > PI/4 && angle < PI/2 && deltaX < 0 && deltaY < 0) ){
        direction = "South";
    }
    
    else if( (angle <= PI/2 && angle >= PI/4 && deltaX > 0 && deltaY > 0) || (angle < -PI/4 && angle > -PI/2 && deltaX < 0 && deltaY > 0)){
        direction = "North";
    }
    
    else if( (angle <= PI/4 && angle >= 0 && deltaX >= 0 && deltaY >= 0) || (angle >= -PI/4 && angle < 0 && deltaX > 0 && deltaY < 0) ){
        direction = "East";
    }
    
    else{
        direction = "West";
    }

    std::cout << direction << std::endl;
    resultSingle.append("Head ");
    resultSingle.append(direction);
    resultSingle.append(" on ");
    resultSingle.append( gStreetsVector[gSegmentVector[pathSegments[0]].myStreet].streetName);
    resultSingle.append("\n");
    pathInstruction.push_back(resultSingle);
    // for one segment paths. //

    if(pathSegments.size() > 1){

        pathSegments.push_back(-1);
        StreetSegmentIdx currentSeg = pathSegments[0];  // always starts at an intersection.
        StreetSegmentIdx previousSeg;

        int index = 1;

        // loop thru pathsegments vector. if the next entry's street name is the same as the previous
        while(index < pathSegments.size() - 1 ) {
            std::string result;

            int currentDistance = gSegmentVector[currentSeg].segLength;
            previousSeg = currentSeg;
            currentSeg = pathSegments[index];

            if (currentSeg != -1) {
                while (gSegmentVector[currentSeg].myStreet == gSegmentVector[previousSeg].myStreet &&
                       currentSeg != -1) {

                    currentDistance += gSegmentVector[currentSeg].segLength;
                    index++;

                    previousSeg = currentSeg;
                    currentSeg = pathSegments[index];

                    if (currentSeg == -1) break;
                }
            }
            result.append("Travel on ");
            result.append(gStreetsVector[gSegmentVector[previousSeg].myStreet].streetName);
            result.append(" for ");
            result.append(std::to_string(currentDistance));
            result.append(" meters\n");


            if (currentSeg != -1) {
                result.append("Go ");

//                determineDirection( gIntersectVector[findIntersectionsOfTwoStreets(currentSeg, previousSeg)[0]].intersectXYLoc.x,
//                                    gIntersectVector[findIntersectionsOfTwoStreets(currentSeg, previousSeg)[0]].intersectXYLoc.y,)

                IntersectionIdx trueFrom2;
                IntersectionIdx trueCurrentIntersect2;
                IntersectionIdx trueTo2;
                
                if(gSegmentVector[currentSeg].from == gSegmentVector[previousSeg].to){
                    trueCurrentIntersect2 = gSegmentVector[previousSeg].to;
                    trueTo2 = gSegmentVector[currentSeg].to;
                    trueFrom2 = gSegmentVector[previousSeg].from;
                }
                else if(gSegmentVector[currentSeg].to == gSegmentVector[previousSeg].to){
                    trueCurrentIntersect2 = gSegmentVector[previousSeg].to;
                    trueTo2 = gSegmentVector[currentSeg].from;
                    trueFrom2 = gSegmentVector[previousSeg].from;

                }
                else if(gSegmentVector[currentSeg].from == gSegmentVector[previousSeg].from){
                    trueCurrentIntersect2 = gSegmentVector[previousSeg].from;
                    trueTo2 = gSegmentVector[currentSeg].to;
                    trueFrom2 = gSegmentVector[previousSeg].to;
                }else{
                    trueCurrentIntersect2 = gSegmentVector[previousSeg].from;
                    trueTo2 = gSegmentVector[currentSeg].from;
                    trueFrom2 = gSegmentVector[previousSeg].to;
                }
                
                result.append(determineDirection(gIntersectVector[trueFrom2].intersectXYLoc.x,
                                                 gIntersectVector[trueFrom2].intersectXYLoc.y,
                                                 gIntersectVector[trueCurrentIntersect2].intersectXYLoc.x,
                                                 gIntersectVector[trueCurrentIntersect2].intersectXYLoc.y,
                                                 gIntersectVector[trueTo2].intersectXYLoc.x,
                                                 gIntersectVector[trueTo2].intersectXYLoc.y));
                
//                result.append(determineDirection(gIntersectVector[gSegmentVector[previousSeg].from].intersectXYLoc.x,
//                                                 gIntersectVector[gSegmentVector[previousSeg].from].intersectXYLoc.y,
//                                                 gIntersectVector[gSegmentVector[previousSeg].to].intersectXYLoc.x,
//                                                 gIntersectVector[gSegmentVector[previousSeg].to].intersectXYLoc.y,
//                                                 gIntersectVector[gSegmentVector[currentSeg].to].intersectXYLoc.x,
//                                                 gIntersectVector[gSegmentVector[previousSeg].to].intersectXYLoc.y));
                result.append(" onto ");
                result.append(gStreetsVector[gSegmentVector[currentSeg].myStreet].streetName);
                result.append("\n");
            }

            pathInstruction.push_back(result);
       
            if(currentSeg != -1){
                index++;
            }
        }
    }
    pathInstruction.push_back("destination reached\n");
        
}

std::string determineDirection (int Ax, int Ay, int Bx, int By, int Cx, int Cy){

    if( (Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax) > 15){
        return "left";
    }else if((Bx - Ax) * (Cy - Ay) - (By - Ay) * (Cx - Ax) < -15){
        return "right";
    }else{
        return "straight";
    }

}

void displayFirstInstructions (IntersectionIdx search1, IntersectionIdx search2, ezgl::application *application){
    if(search1 != -1 && search2 != -1) {
        std::string text_display;
        int numInstructions = 3;

        text_display.append("Current Instructions:\n");

        GtkTextBuffer *text_buffer = (GtkTextBuffer *) application->get_object("PathInstructionBuffer");
        //DISPLAY PATH INSTRUCTIONS TO BUFFER

        if (pathInstruction.size() < 3) {
            numInstructions = pathInstruction.size();
        }

        for (int i = instructionCounter; i < instructionCounter + numInstructions; i++) {
            text_display.append(pathInstruction[i]);
        }

        int length = text_display.length();
        gtk_text_buffer_set_text(text_buffer, text_display.c_str(), length);
    }
}
