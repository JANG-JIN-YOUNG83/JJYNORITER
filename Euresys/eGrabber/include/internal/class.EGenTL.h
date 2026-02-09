    public:
        bool isShared() const {
            return shared;
        }
        void eventKillQuiet(gc::EVENT_HANDLE eh) {
            impl.eventKillQuiet(eh);
        }
    private:
        Internal::GenTLImpl impl;
        const bool shared;
        Internal::ConcurrencyLock mutex;
        // TL_HANDLE
        Internal::RefCounted<gc::TL_HANDLE> tl;
        // IF_HANDLE
        typedef std::map<const std::string, Internal::RefCounted<gc::IF_HANDLE> > if_map_type;
        if_map_type ifs;
        // DEV_HANDLE
        typedef std::map<const std::string, Internal::RefCounted<gc::DEV_HANDLE> > dev_map_type;
        std::map<gc::IF_HANDLE, dev_map_type> devs;
        // DS_HANDLE
        typedef std::map<const std::string, Internal::RefCounted<gc::DS_HANDLE> > ds_map_type;
        std::map<gc::DEV_HANDLE, ds_map_type> dss;
    public:
        ETraceContext &traceCtx;
    private:
        EGenTL(const EGenTL&); // EGenTL objects cannot be copied
        EGenTL &operator=(const EGenTL&); // EGenTL objects cannot be copied
