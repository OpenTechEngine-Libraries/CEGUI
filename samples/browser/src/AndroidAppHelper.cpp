
#include "AndroidAppHelper.h"
#ifdef CEGUI_SAMPLES_ANDROID_GLES2
#   include "CEGuiEGLSharedBase.h"
#   include "SampleBrowserBase.h"
#elif defined(CEGUI_SAMPLES_ANDROID_OGRE)
AndroidInputInjector* AndroidAppHelper::mInputInjector = NULL;
AndroidMultiTouch* AndroidAppHelper::mTouch = NULL;
AndroidKeyboard* AndroidAppHelper::mKeyboard = NULL;
Ogre::FileSystemLayer* AndroidAppHelper::mFSLayer; // File system abstraction layer
OIS::MultiTouchListener* AndroidAppHelper::mMultiTouchListener = NULL;
OIS::KeyListener* AndroidAppHelper::mKeyListener = NULL;
Ogre::RenderWindow* AndroidAppHelper::mRenderWnd = NULL;
#endif

bool AndroidAppHelper::d_windowCreated = false;

#ifdef CEGUI_SAMPLES_ANDROID_GLES2
//----------------------------------------------------------------------------//
int32_t AndroidAppHelper::handleInput (struct android_app* app, AInputEvent* event)
{
    static int prev_x = -1;
    static int prev_y = -1;
    if (true)
    {
        if (AInputEvent_getType (event) == AINPUT_EVENT_TYPE_MOTION)
        {
            int action = (int) (AMOTION_EVENT_ACTION_MASK & AMotionEvent_getAction (event));
            float x = AMotionEvent_getRawX (event, 0);
            float y = AMotionEvent_getRawY (event, 0);
            if (action == AMOTION_EVENT_ACTION_MOVE)
            {
                if (prev_x > 0 || prev_y > 0)
                {
                    float delta_x = x - prev_x;
                    float delta_y = y - prev_x;
                    CEGuiEGLSharedBase::getSingleton().getSampleApp()->injectMousePosition (
                        static_cast<float> (x), static_cast<float> (y));
                }
            }
            else if (action == AMOTION_EVENT_ACTION_DOWN)
            {
                CEGuiEGLSharedBase::getSingleton().getSampleApp()->injectMousePosition (x, y);
                CEGuiEGLSharedBase::getSingleton().getSampleApp()->injectMouseButtonDown (CEGUI::LeftButton);
            }
            else if (action == AMOTION_EVENT_ACTION_UP)
            {
                CEGuiEGLSharedBase::getSingleton().getSampleApp()->injectMouseButtonUp (CEGUI::LeftButton);
            }
            prev_x = x;
            prev_y = y;
        }
        else if (AInputEvent_getType (event) == AINPUT_EVENT_TYPE_KEY)
        {
            //keyboard injection
        }

        return 1;
    }
    return 0;

}

#elif defined(CEGUI_SAMPLES_ANDROID_OGRE)
int32_t AndroidAppHelper::handleInput (struct android_app* app, AInputEvent* event)
{
    if (mInputInjector)
    {
        if (AInputEvent_getType (event) == AINPUT_EVENT_TYPE_MOTION)
        {
            int action = (int) (AMOTION_EVENT_ACTION_MASK & AMotionEvent_getAction (event));

            if (action == 0)
            {
                mInputInjector->injectTouchEvent (2, AMotionEvent_getRawX (event, 0),
                                                  AMotionEvent_getRawY (event, 0));
            }
            mInputInjector->injectTouchEvent (action, AMotionEvent_getRawX (event, 0),
                                              AMotionEvent_getRawY (event, 0));
        }
        else
        {
            mInputInjector->injectKeyEvent (AKeyEvent_getAction (event), AKeyEvent_getKeyCode (event));
        }

        return 1;
    }
    return 0;
}


#endif

#ifdef CEGUI_SAMPLES_ANDROID_GLES2
//----------------------------------------------------------------------------//
void AndroidAppHelper::handleCmd (struct android_app* app, int32_t cmd)
{
    switch (cmd)
    {
    case APP_CMD_SAVE_STATE:
        break;
    case APP_CMD_INIT_WINDOW:
        __android_log_write (ANDROID_LOG_DEBUG, "EGL", "handle command APP_CMD_INIT_WINDOW");
        if (app->window != NULL)
        {
            __android_log_write (ANDROID_LOG_DEBUG, "EGL", "handle command APP_CMD_INIT_WINDOW win is NULL");
            if (!isWindowCreated())
            {
                __android_log_write (ANDROID_LOG_DEBUG, "EGL", "handle command APP_CMD_INIT_WINDOW Init");
                CEGuiEGLSharedBase::getSingleton().Init (app->window);
                setWindowCreated (true);
            }
            else
            {
                __android_log_write (ANDROID_LOG_DEBUG, "EGL", "handle command APP_CMD_INIT_WINDOW Resume");
                CEGuiEGLSharedBase::getSingleton().Resume (app->window);
            }
        }
        else
        {
            __android_log_write (ANDROID_LOG_DEBUG, "EGL", "handle command APP_CMD_INIT_WINDOW win is NOT NULL");
            __android_log_write (ANDROID_LOG_DEBUG, "EGL", "HOW ARE WE HERE?????????????");
        }
        break;
    case APP_CMD_TERM_WINDOW:
        CEGuiEGLSharedBase::getSingleton().Terminate();
        break;
    case APP_CMD_GAINED_FOCUS:
        CEGuiEGLSharedBase::getSingleton().Resume (app->window);
        break;
    case APP_CMD_LOST_FOCUS:
        CEGuiEGLSharedBase::getSingleton().Suspend();
        CEGuiEGLSharedBase::getSingleton().engine_draw_frame();
        break;
    }
}
#elif defined(CEGUI_SAMPLES_ANDROID_OGRE)
//----------------------------------------------------------------------------//
void AndroidAppHelper::handleCmd (struct android_app* app, int32_t cmd)
{
    switch (cmd)
    {
    case APP_CMD_SAVE_STATE:
        break;
    case APP_CMD_INIT_WINDOW:
        __android_log_write (ANDROID_LOG_ERROR, "CEGUIDEMO", "-------------- APP_CMD_INIT_WINDOW");
        if (app->window && Ogre::Root::getSingletonPtr())
        {
            AConfiguration* config = AConfiguration_new();
            AConfiguration_fromAssetManager (config, app->activity->assetManager);

            if (!mRenderWnd)
            {
                __android_log_write (ANDROID_LOG_ERROR, "CEGUIDEMO", "------- create render window");
                Ogre::NameValuePairList opt;
                opt["externalWindowHandle"] = Ogre::StringConverter::toString ((int)app->window);
                opt["androidConfig"] = Ogre::StringConverter::toString ((int)config);
                mRenderWnd = Ogre::Root::getSingleton().createRenderWindow (
                                 "OgreWindow", 0, 0, false, &opt);
                setWindowCreated (true);
                __android_log_write (ANDROID_LOG_ERROR, "CEGUIDEMO", "------- render window created");
            }
            else
            {
                __android_log_write (ANDROID_LOG_ERROR, "CEGUIDEMO", "------- create internal res");
                static_cast<Ogre::AndroidEGLWindow*> (mRenderWnd)->_createInternalResources ( app->window, config);
                __android_log_write (ANDROID_LOG_ERROR, "CEGUIDEMO", "------- internal res created");
            }
            AConfiguration_delete (config);
        }
        break;
    case APP_CMD_TERM_WINDOW:
        if (Ogre::Root::getSingletonPtr() && mRenderWnd)
            static_cast<Ogre::AndroidEGLWindow*> (mRenderWnd)->_destroyInternalResources();
        break;
    case APP_CMD_GAINED_FOCUS:
        break;
    case APP_CMD_LOST_FOCUS:
        break;
    case APP_CMD_CONFIG_CHANGED:
        break;
    }
}
#endif

bool AndroidAppHelper::processEvents()
{
    int ident, events;
    struct android_poll_source* source;
    struct android_app* state = CEGUI::AndroidUtils::getAndroidApp();
    while ((ident = ALooper_pollAll (0, NULL, &events, (void**)&source)) >= 0)
    {
        if (source != NULL)
            source->process (state, source);
        if (state->destroyRequested != 0)
            return false;
    }
    return true;
}
#ifdef CEGUI_SAMPLES_ANDROID_OGRE
void AndroidAppHelper::go()
{
    while (true)
    {
        bool processResult = processEvents();
        Ogre::WindowEventUtilities::messagePump();
        if (mRenderWnd != NULL && mRenderWnd->isActive())
        {
            mRenderWnd->windowMovedOrResized();
            Ogre::Root::getSingletonPtr()->renderOneFrame();
        }
    }
}
#endif

//----------------------------------------------------------------------------//
bool AndroidAppHelper::createWindow()
{
    int ident, events;
    struct android_poll_source* source;
    struct android_app* state = CEGUI::AndroidUtils::getAndroidApp();
    state->onAppCmd = handleCmd;
    while (!d_windowCreated)
    {
        //This will process until the android window is created.
        while ((ident = ALooper_pollAll (0, NULL, &events, (void**)&source)) >= 0)
        {
            if (source)
                source->process (state, source);
            if (state->destroyRequested != 0)
            {
                return false;
            }
        }
    }
    state->onInputEvent = handleInput;
    return true;
}

//----------------------------------------------------------------------------//
bool AndroidAppHelper::isWindowCreated()
{
    return d_windowCreated;
}

//----------------------------------------------------------------------------//
void AndroidAppHelper::setWindowCreated (bool windowCreated)
{
    d_windowCreated = windowCreated;
}

#ifdef CEGUI_SAMPLES_ANDROID_OGRE
//----------------------------------------------------------------------------//
AndroidMultiTouch::AndroidMultiTouch(): OIS::MultiTouch ("Ogre", false, 0, 0)
{
}

//----------------------------------------------------------------------------//
void AndroidMultiTouch::setBuffered (bool buffered)
{
}

//----------------------------------------------------------------------------//
void AndroidMultiTouch::capture() {}

//----------------------------------------------------------------------------//
OIS::Interface* AndroidMultiTouch::queryInterface (OIS::Interface::IType type)
{
    return 0;
}

//----------------------------------------------------------------------------//
void AndroidMultiTouch::_initialize() {}

//----------------------------------------------------------------------------//
OIS::MultiTouchState& AndroidMultiTouch::getMultiTouchState (int i)
{
    while (i >= mStates.size())
    {
        Ogre::RenderWindow* pRenderWnd =
            static_cast<Ogre::RenderWindow*> (Ogre::Root::getSingleton().getRenderTarget ("OgreWindow"));
        if (pRenderWnd)
        {
            OIS::MultiTouchState state;
            state.width = pRenderWnd->getWidth();
            state.height = pRenderWnd->getHeight();
            mStates.push_back (state);
        }
    }
    return mStates[i];
}

//----------------------------------------------------------------------------//
AndroidKeyboard::AndroidKeyboard(): OIS::Keyboard ("Ogre", false, 1, 0)
{
}

//----------------------------------------------------------------------------//
void AndroidKeyboard::setBuffered (bool buffered)
{
}

//----------------------------------------------------------------------------//
void AndroidKeyboard::capture()
{
}

//----------------------------------------------------------------------------//
OIS::Interface* AndroidKeyboard::queryInterface (OIS::Interface::IType type)
{
    return 0;
}

//----------------------------------------------------------------------------//
void AndroidKeyboard::_initialize()
{
}

//----------------------------------------------------------------------------//
bool AndroidKeyboard::isKeyDown (OIS::KeyCode key) const
{
    return false;
}

//----------------------------------------------------------------------------//
const std::string& AndroidKeyboard::getAsString (OIS::KeyCode kc)
{
    static std::string defstr = "";
    return defstr;
}

//----------------------------------------------------------------------------//
void AndroidKeyboard::copyKeyStates (char keys[256]) const
{
}

//----------------------------------------------------------------------------//
AndroidInputInjector::AndroidInputInjector (OIS::KeyListener *keyListener, OIS::MultiTouchListener *touchListener,
        AndroidMultiTouch* touch, AndroidKeyboard* keyboard)
    : keyListener (keyListener), touchListener (touchListener), mTouch (touch), mKeyboard (keyboard)
{
}

//----------------------------------------------------------------------------//
void AndroidInputInjector::injectKeyEvent (int action, int32_t keyCode)
{
    if (keyCode == AKEYCODE_BACK)
    {
        OIS::KeyEvent evt (mKeyboard, OIS::KC_ESCAPE, 0);
        if (action == 0)
        {
            keyListener->keyPressed (evt);
        }
        else
        {
            keyListener->keyReleased (evt);
        }
    }
}

//----------------------------------------------------------------------------//
void AndroidInputInjector::injectTouchEvent (int action, float x, float y, int pointerId)
{
    OIS::MultiTouchState &state = mTouch->getMultiTouchState (pointerId);

    switch (action)
    {
    case 0:
        state.touchType = OIS::MT_Pressed;
        break;
    case 1:
        state.touchType = OIS::MT_Released;
        break;
    case 2:
        state.touchType = OIS::MT_Moved;
        break;
    case 3:
        state.touchType = OIS::MT_Cancelled;
        break;
    default:
        state.touchType = OIS::MT_None;
    }

    if (state.touchType != OIS::MT_None)
    {
        int last = state.X.abs;
        state.X.abs = (int)x;
        state.X.rel = state.X.abs - last;

        last = state.Y.abs;
        state.Y.abs = (int)y;
        state.Y.rel = state.Y.abs - last;

        state.Z.abs = 0;
        state.Z.rel = 0;

        OIS::MultiTouchEvent evt (mTouch, state);

        switch (state.touchType)
        {
        case OIS::MT_Pressed:
            touchListener->touchPressed (evt);
            break;
        case OIS::MT_Released:
            touchListener->touchReleased (evt);
            break;
        case OIS::MT_Moved:
            touchListener->touchMoved (evt);
            break;
        case OIS::MT_Cancelled:
            touchListener->touchCancelled (evt);
            break;
        default:
            break;
        }
    }
}
//----------------------------------------------------------------------------//
void AndroidAppHelper::shutdown()
{
    OGRE_DELETE_T (mFSLayer, FileSystemLayer, Ogre::MEMCATEGORY_GENERAL);
    mRenderWnd = NULL;
    mKeyListener = NULL;
    mMultiTouchListener = NULL;

    delete mTouch;
    mTouch = NULL;

    delete mKeyboard;
    mKeyboard = NULL;

    delete mInputInjector;
    mInputInjector = NULL;
}

void AndroidAppHelper::initialiseFS()
{
    mFSLayer = OGRE_NEW_T (Ogre::FileSystemLayer, Ogre::MEMCATEGORY_GENERAL) (OGRE_VERSION_NAME);
    AAssetManager* assetManager =  CEGUI::AndroidUtils::getAndroidApp()->activity->assetManager;
    Ogre::APKFileSystemArchiveFactory* apk = new Ogre::APKFileSystemArchiveFactory (assetManager);
    Ogre::APKZipArchiveFactory* zip = new Ogre::APKZipArchiveFactory (assetManager);
    Ogre::ArchiveManager::getSingleton().addArchiveFactory (apk);
    Ogre::ArchiveManager::getSingleton().addArchiveFactory (zip);
}

bool AndroidAppHelper::initialiseOIS (OIS::KeyListener* keyListener,
        OIS::MultiTouchListener* mMultiTouchListener)
{
    if (!mTouch)
        mTouch = new AndroidMultiTouch();
    if (!mKeyboard)
        mKeyboard = new AndroidKeyboard();
    if (!mInputInjector)
        mInputInjector = new AndroidInputInjector (mKeyListener, mMultiTouchListener, mTouch, mKeyboard);
}
Ogre::RenderWindow* AndroidAppHelper::getRenderWindow ()
{
    return mRenderWnd;
}

#endif

//----------------------------------------------------------------------------//

