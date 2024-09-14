#pragma once

#include "stdafx.h"

#include "Popup.h"

namespace NGMC
{
    //	Class for rendering and handling a popup window used to pick a file object.
    class PopupFilePicker : public Popup
    {
    public:
        //  Constructs a File Picker Popup object.
        PopupFilePicker();

        //  Sets up the File Picker Popup by specifying the root file from which all descendants and itself will be displayed in the list of selectable files, the out file into which the result will be loaded, and the acceptType which defines which FileType is expected for the out file. If set to FileTypeId::unknown, all file types are accepted.
        void Setup(File* p_RootFile, File** p_OutFile, FileType acceptType = FileType());

    private:
        //  This function renders and handles the specified file in the list of selectable files.
        void OnRenderFile(File* p_File, bool isNotClipped);

        //	The overridden function that renders the contents of the Popup window.
        void OnRenderContent() override;

        //  The root file object pointer which is the top-most file object which can be picked, above all its descendants.
        File* m_RootFile;

        //  The out file object pointer address, the out file object pointer will be saved to this address once the user accepts their selection.
        File** m_OutFile;

        //  The FileType which is enforced, all other FileTypes will not be pickable. All files are pickable if the type is FileType::invalid.
        FileType m_AcceptType;

        //  The file object pointer of the file which is currently selected in the File Picker Popup UI.
        File* m_SelectedFile;


        //	The ImGuiListClipper object responsible for clipping the contents of the File Picker Popup to just the visible, greatly helping performance when thousands of files are registered.
        ImGuiListClipper m_Clipper;

        //	Whether the ImGuiListClipper object is still stepping, estimating how many objects will keep fitting in the visible space of the window.
        bool m_IsStepping;

        //	The count of items exposed in the File Picker Popup.
        int m_ListItemCount;
    };
}