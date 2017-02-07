/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#ifndef INC_RESOURCEDIALOG_H
#define INC_RESOURCEDIALOG_H

#include "definitions.h"
#include "resource.h"
#include <afxShellListCtrl.h>
#include <afxeditbrowsectrl.h>
#include "filename.h"
#include "PQMapNode.h"

#include "list.h"
using std::list;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

class ResourceDialog;

class MBrowseControl : public CMFCEditBrowseCtrl
{
public:
	ResourceDialog *parent;

	MBrowseControl(ResourceDialog *dlgParent = NULL) : parent(dlgParent) {}
	virtual void OnAfterUpdate();

};

class ResourceDialog : public CDialog
{
	DECLARE_DYNAMIC(ResourceDialog)

public:
	enum { IDD = IDD_RESOURCEDIALOG };

	ResourceDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~ResourceDialog();
	virtual BOOL OnInitDialog();
	PQResourcePtr ShowDialog(vector<PQResourcePtr> *resources, const char *filename = NULL);
	void AddClassID(string &id);

	PQResourcePtr newResource;

protected:
	friend class MBrowseControl;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	MBrowseControl browseCtrl;
	CComboBox cmbClassIDs;
	CEdit resourceName;

	afx_msg void OnBtnAddResource();
	afx_msg void OnBtnClose();

	vector<PQResourcePtr> *pResources;

	DECLARE_MESSAGE_MAP()

	Filename initialFilename;
public:

	afx_msg void OnClose();
};

#endif /* INC_RESOURCEDIALOG_H */