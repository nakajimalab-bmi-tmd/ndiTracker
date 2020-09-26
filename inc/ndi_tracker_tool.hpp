#ifndef include_ndi_tracker_tool_hpp
#define include_ndi_tracker_tool_hpp

#include <stdint.h>
#include <boost/filesystem.hpp>
#include <Eigen/Geometry>
#include "ndi_types.hpp"

namespace device
{
  namespace ndi
  {

    template<typename Device>
    class tracker;

    template<typename Device>
    class tracker_tool
    {
      friend class tracker<Device>;

    public:
      struct port_handle_information_type
      {
        std::wstring main_type;
        std::wstring manufacture_id;
        std::wstring tool_revision;

        std::wstring part_name;
      };

      typedef Eigen::AffineCompact3f matrix_type;
      tracker_tool() {}
      matrix_type const& get_transform() const;

      void attach_to_tracker(tracker<Device>& arg);
      void attach_to_tracker(tracker<Device>& arg, boost::filesystem::path const& srom_file);

      uint16_t const get_port_handle() const
      {
        return this->port_handle;
      }

      port_handle_information_type const& get_information() const
      {
        return port_handle_information;
      }

      std::wstring const& get

      bool const is_valid() const
      {
        return this->status.is_valid();
      }

      bool const is_missing() const
      {
        return this->status.is_missing();
      }

      bool const is_disabled() const
      {
        return this->status.is_disabled();
      }

      bool const is_modified() const
      {
        return this->modified;
      }

      uint32_t const get_frame_number() const
      {
        return this->frame_number;
      }

    protected:
      void set_transform(handle_data const& arg);
      void set_port_handle(uint16_t const arg)
      {
        this->port_handle = arg;
      }

      port_handle_information_type port_handle_information;

      
      uint16_t port_handle = 0;
      matrix_type matrix;
      handle_status status;
      uint32_t frame_number;
      bool modified = false;
    };

    template<typename Device>
    void tracker_tool<Device>::attach_to_tracker(tracker<Device>& arg)
    {
      arg.attach_tracker_tool(*this);
    }

    template<typename Device>
    void tracker_tool<Device>::attach_to_tracker(tracker<Device>& arg, boost::filesystem::path const& srom_path)
    {

    }

    template<typename Device>
    void tracker_tool<Device>::set_transform(handle_data const& arg)
    {
      using namespace Eigen;

      this->status = arg.status;

      if (this->status.is_valid())
      {
        this->matrix = Translation3f(arg.transform_data.x, arg.transform_data.y, arg.transform_data.z) * Quaternionf(arg.transform_data.w, arg.transform_data.qx, arg.transform_data.qy, arg.transform_data.qz);
      }

      modified = false;
      if (this->frame_number != arg.transform_data.frame)
      {
        modified = true;
        this->frame_number = arg.transform_data.frame;
      }
    }

    template<typename Device>
    typename tracker_tool<Device>::matrix_type const& tracker_tool<Device>::get_transform() const
    {
      return this->matrix;
    }
  }
}

#endif
