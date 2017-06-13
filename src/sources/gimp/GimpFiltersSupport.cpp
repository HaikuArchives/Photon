#include "Flags.h"
#if __x86
#include <stl.h>
#else
#include <vector.h>
#endif
#include "Document.h"
#include "Exceptions.h"
#include "GraphicsTypes.h"
#include "BaseFilter.h"
#include "stdio.h"
#include "PaintApp.h"
#include <StorageKit.h>
#include <sys/stat.h>
#include "PaintApp.h"

//-------------------------------------------------------------------------------------------------------------
//	***********	WARNING ***************
//	The following structure is also in GimpStub.h, You need to change it at both places, and be very cautious.
//-------------------------------------------------------------------------------------------------------------
#define VERSION_NUMBER	0x100
extern "C" {
typedef void (*GInstallGimp) (BPath *path, InitInfo *info);
typedef void (*GMenuInstaller) (const char *menu, const char *item, BMessage *msg);
	void	GimpMenuInstaller (const char *menu, const char *item, BMessage *msg);
};

struct InitInfo {
	int32			fVersion;
	BApplication	*fApp;
	GMenuInstaller	*fInstaller;
};

void GimpMenuInstaller (const char *menu, const char *item, BMessage *msg) {
	gMyApp->AddFilterPlugin (menu, item, msg);
}

static void RecursInstallFilters (BDirectory *dir) {
	InitInfo	info;
	info.fVersion = VERSION_NUMBER; info.fApp = be_app; info.fInstaller = GimpMenuInstaller;
	do {
		BEntry	entry;
		if (B_OK != dir->GetNextEntry (&entry, true)) break;
		struct stat	entryStat;		//	entryStat.st_mode == S_ISREG S_ISDIR
		ASSERT (entry.Exists ());
		if (B_OK == entry.GetStat (&entryStat)) {
			if (S_ISREG(entryStat.st_mode)) {
				BPath		entryPath;
				if (B_OK == entry.GetPath (&entryPath)) {
					image_id	addon_image;
					addon_image = load_add_on (entryPath.Path());
					if (long(addon_image) > 0) {
						status_t		status;
						GInstallGimp	funcPtr;
						status = get_image_symbol (addon_image, "InstallGimp", B_SYMBOL_TYPE_TEXT, (void **)&funcPtr);
						ASSERT (status == B_OK);
						(*funcPtr) (&entryPath, &info);
					}
				} else {
					ASSERT (false);
				}
			} else if (S_ISDIR(entryStat.st_mode)) {
				BDirectory	dir2Parse (&entry);
				RecursInstallFilters (&dir2Parse);
			} else {
				ASSERT (false);
			}
		} else {
			ASSERT (false);
		}
	} while (true);
}

void RegisterGimpFilters (TPaintApp *app) {
	app_info	appInfo;
	be_app->GetAppInfo (&appInfo);
	entry_ref	filtersRef = appInfo.ref;
	filtersRef.set_name ("GIMP");
	BDirectory	filtersDir (&filtersRef);

	RecursInstallFilters (&filtersDir);
}

void AddFilterPlugin (const char *menu, const char *item, BMessage *archive) {
	gMyApp->AddFilterPlugin (menu, item, archive);
}