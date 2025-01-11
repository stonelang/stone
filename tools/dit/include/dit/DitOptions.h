#ifndef DIT_OPTIONS_H
#define DIT_OPTIONS_H



namespace dit {

	class DitOptions final {

	public:

        enum class Command {
            None = 0,
            InitProject,
            AddPackage,
            AddModule,
            AddFile,
            RemoveProject,
            RemovePackage,
            RemoveModule,
            RemoveFile,
            Publish,
            Sync,
            Build
        };
	};
}