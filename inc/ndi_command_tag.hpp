#ifndef include_ndi_command_tag_hpp
#define include_ndi_command_tag_hpp

namespace device {

  namespace ndi {

    class polaris;
    class aurora;

    namespace traits
    {

      // recv_tag
      struct ascii_reply_tag {};
      struct binary_reply_tag {};

      // postprocess_tag
      struct has_postprocess_tag {};

      template <typename command> struct tag {};

      template <typename command>
      struct timeout
      {
        static constexpr uint32_t value()
        {
          return 3000;
        }
      };
      
    }
  }
}
#endif
