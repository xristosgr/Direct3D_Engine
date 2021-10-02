
static int NR_LIGHTS = 128;
static const unsigned int MAX_NO_OF_LIGHTS = 50;
static const unsigned int MAX_NO_OF_SHADOWS = 12;

static int window_width = 1600;
static int window_height = 900;
static int SHADOWMAP_WIDTH = 1024;
static int SHADOWMAP_HEIGHT = 1024;
static int SUN_SHADOWMAP_WIDTH = 1024;
static int SUN_SHADOWMAP_HEIGHT = 1024;
static int bloomTextureWidth = 1024;
static int bloomTextureHeight = 1024;
static int textureWidth = 100;
static int textureHeight = 100;
static int downSampleWidth = SHADOWMAP_WIDTH/2;
static int downSampleHeight = SHADOWMAP_HEIGHT/2;
static int bloomdownSampleWidth = bloomTextureWidth / 2;
static int bloomdownSampleHeight = bloomTextureHeight / 2;