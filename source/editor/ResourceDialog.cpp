/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#include "ResourceDialog.h"
#include "afxdialogex.h"
#include "PQMap.h"

IMPLEMENT_DYNAMIC(ResourceDialog, CDialog)

BEGIN_MESSAGE_MAP(ResourceDialog, CDialog)
	ON_COMMAND(IDC_ADDRESOURCE, OnBtnAddResource)
	ON_COMMAND(IDC_CLOSE, OnBtnClose)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void MBrowseControl::OnAfterUpdate()
{
	CString f;
	GetWindowText(f);
	Filename fn = f;
	string name = fn.GetName();

	if(fn.GetExtension() != "")
		name.erase(name.size() - 4, 4);

	parent->resourceName.SetWindowText( name.c_str() );
}

ResourceDialog::ResourceDialog(CWnd* pParent) : CDialog(ResourceDialog::IDD, pParent)
{
	
}

ResourceDialog::~ResourceDialog()
{

}

void ResourceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BROWSE1, browseCtrl);
	DDX_Control(pDX, IDC_NAME, resourceName);
	DDX_Control(pDX, IDC_CLASSID, cmbClassIDs);
}

BOOL ResourceDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	browseCtrl.parent = this;

	cmbClassIDs.Clear();

	// add all the class IDs to the combo box
	for(unsigned int i = 0; i < pResources->size(); i++)
	{
		bool exists = false;
		CString str;
		for(int j = 0; j < cmbClassIDs.GetCount(); j++)
		{
			cmbClassIDs.GetLBText(j, str);
			if(str == pResources->at(i)->class_id)
			{
				exists = true;
				break;
			}
		}

		if(!exists)
			cmbClassIDs.AddString(pResources->at(i)->class_id);
	}

	// if this dialog was created by mouseDown a resource into the 
	if(initialFilename != "")
	{
		browseCtrl.SetWindowTextA( initialFilename.c_str() );
		
		string name = initialFilename.GetName();
		
		if(initialFilename.GetExtension() != "")
			name.erase(name.size() - 4, 4);

		resourceName.SetWindowText( name.c_str() );
	}

	return TRUE;
}

PQResourcePtr ResourceDialog::ShowDialog(vector<PQResourcePtr> *resources, const char *filename)
{
	if(!resources)
		return PQResourcePtr();

	pResources = resources;

	if(filename != NULL)
		initialFilename = filename;

	newResource.reset();

	DoModal();
	
	return newResource;
}

void ResourceDialog::OnBtnAddResource()
{	
	try
	{
		CString fn;
		browseCtrl.GetWindowText(fn);
		Filename filename = fn;

		if(filename == "")
			throw "Please select a source file for this resource.";

		if(!filename.Exists())
			throw "The specified file does not exist.";

		CString resName;
		resourceName.GetWindowText(resName);
		if(resName == "")
			throw "Please specify a unique name for this resource.";

		bool resNameExists = false;
		for(Uint16 i = 0; i < pResources->size(); i++)
		{
			if(resName == pResources->at(i)->resource_name)
			{
				resNameExists = true;
				break;
			}
		}

		if(resNameExists == true)
			throw "The specified resource name is already in use.";

		CString classID;
		cmbClassIDs.GetWindowText(classID);
		if(classID == "")
			throw "Please specify a class ID for this resource.";

		string ext = filename.GetExtension();
		if(ext == "png")
		{
			newResource.reset( new PQResImage(filename, (const char*)resName, (const char*)classID) );
		}
		else if(ext == "wav")
		{
			newResource.reset( new PQResSound(filename, (const char*)resName, (const char*)classID) );
		}
		else
		{
			throw "The specified file type is not supported.\nThe following resource types are supported: *.wav, *.png";
		}

		EndDialog(0);
	}

	catch(const char *error)
	{
		MessageBox(error, "Error", MB_OK | MB_ICONERROR);
		EndDialog(-1);
	}
}

void ResourceDialog::OnBtnClose()
{
	EndDialog(-1);
}

void ResourceDialog::OnClose()
{
	EndDialog(-1);
}
