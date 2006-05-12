/***********************************************************************
	filename: 	CEGUIScrolledContainer.h
	created:	1/3/2005
	author:		Paul D Turner
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#ifndef _CEGUIScrolledContainer_h_
#define _CEGUIScrolledContainer_h_

#include "CEGUIWindow.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIScrolledContainerProperties.h"
#include <map>

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Helper container window class which is used in the implementation of the
        ScrollablePane widget class.
    */
    class CEGUIEXPORT ScrolledContainer : public Window
    {
    public:
        /*************************************************************************
            Constants
        *************************************************************************/
        static const String WidgetTypeName;     //!< Type name for ScrolledContainer.
        static const String EventNamespace;     //!< Namespace for global events
        static const String EventContentChanged;    //!< Event fired whenever some child changes.
        static const String EventAutoSizeSettingChanged;    //!< Event fired when the autosize setting changes.

        /*************************************************************************
        	Object construction and destruction
        *************************************************************************/
        /*!
        \brief
            Constructor for ScrolledContainer objects.
        */
        ScrolledContainer(const String& type, const String& name);

        /*!
        \brief
            Destructor for ScrolledContainer objects.
        */
        ~ScrolledContainer(void);

        /*************************************************************************
        	Public interface methods
        *************************************************************************/
        /*!
        \brief
            Return whether the content pane is auto sized.

        \return
            - true to indicate the content pane will automatically resize itself.
            - false to indicate the content pane will not automatically resize itself.
        */
        bool isContentPaneAutoSized(void) const;

        /*!
        \brief
            Set whether the content pane should be auto-sized.

        \param setting
            - true to indicate the content pane should automatically resize itself.
            - false to indicate the content pane should not automatically resize itself.

        \return 
            Nothing.
        */
        void setContentPaneAutoSized(bool setting);

        /*!
        \brief
            Return the current content pane area for the ScrolledContainer.

        \return
            Rect object that details the current pixel extents of the content
            pane represented by this ScrolledContainer.
        */
        const Rect& getContentArea(void) const;

        /*!
        \brief
            Set the current content pane area for the ScrolledContainer.

        \note
            If the ScrolledContainer is configured to auto-size the content pane
            this call will have no effect.

        \param area
            Rect object that details the pixel extents to use for the content
            pane represented by this ScrolledContainer.

        \return
            Nothing.
        */
        void setContentArea(const Rect& area);

        /*!
        \brief
            Return the current extents of the attached content.

        \return
            Rect object that describes the pixel extents of the attached
            child windows.  This is effectively the smallest bounding box
            that could contain all the attached windows.
        */
        Rect getChildExtentsArea(void) const;

    protected:
        /*************************************************************************
        	Implementation methods
        *************************************************************************/
		/*!
		\brief
			Return whether this window was inherited from the given class name at some point in the inheritance heirarchy.

		\param class_name
			The class name that is to be checked.

		\return
			true if this window was inherited from \a class_name. false if not.
		*/
		virtual bool	testClassName_impl(const String& class_name) const
		{
			if (class_name=="ScrolledContainer")	return true;
			return Window::testClassName_impl(class_name);
		}

        /*************************************************************************
        	Implementation of abstract methods from Window
        *************************************************************************/
        void drawSelf(float z) {};

        /*************************************************************************
        	Event trigger methods.
        *************************************************************************/
        /*!
        \brief
            Notification method called whenever the content size may have changed.

        \param e
            WindowEventArgs object.

        \return
            Nothing.
        */
        virtual void onContentChanged(WindowEventArgs& e);

        /*!
        \brief
            Notification method called whenever the setting that controls whether
            the content pane is automatically sized is changed.

        \param e
            WindowEventArgs object.

        \return
            Nothing.
        */
        virtual void onAutoSizeSettingChanged(WindowEventArgs& e);

        /*************************************************************************
        	Event callbacks
        *************************************************************************/
        /*!
        \brief
            Method which receives notifications about child windows being moved.
        */
        bool handleChildSized(const EventArgs& e);

        /*!
        \brief
            Method which receives notifications about child windows being sized.
        */
        bool handleChildMoved(const EventArgs& e);

        /*************************************************************************
        	Overridden from Window.
        *************************************************************************/
        Rect getUnclippedInnerRect(void) const;
        void onChildAdded(WindowEventArgs& e);
        void onChildRemoved(WindowEventArgs& e);
        void onParentSized(WindowEventArgs& e);

        /*************************************************************************
        	Data fields
        *************************************************************************/
        typedef std::multimap<Window*, Event::Connection>  ConnectionTracker;
        ConnectionTracker d_eventConnections;   //!< Tracks event connections we make.
        Rect d_contentArea;     //!< Holds extents of the content pane.
        bool d_autosizePane;    //!< true if the pane auto-sizes itself.

    private:
	    /*************************************************************************
		    Static Properties for this class
	    *************************************************************************/
	    static ScrolledContainerProperties::ContentPaneAutoSized	d_autoSizedProperty;
	    static ScrolledContainerProperties::ContentArea             d_contentAreaProperty;
	    static ScrolledContainerProperties::ChildExtentsArea        d_childExtentsAreaProperty;

	    /*************************************************************************
		    Private methods
	    *************************************************************************/
	    void addScrolledContainerProperties(void);
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIScrolledContainer_h_
