#ifndef include_ndi_types_hpp
#define include_ndi_types_hpp

#include <array>
#include <bitset>
#include <map>
#include <vector>

#include <boost/dynamic_bitset.hpp>

namespace device {
  namespace ndi {


    struct version_information
    {
      std::string type_of_firmware;
      std::string ndi_serial_number;
      std::string characterization_date;
      std::string freeze_tag;
      std::string freeze_date;
      std::string copyright_information;
      std::string combined_firmware_revision;

      //device_type get_device_type();
    };

    class handle_status
    {
      enum possible_value
      {
        UNOCCUPIED = 0,
        VALID = 1,
        MISSING = 2,
        DISABLED = 4,
      };

    public:
      handle_status() : value(UNOCCUPIED) {}
      explicit handle_status(unsigned short const value) : value(static_cast<possible_value>(value)) {}

      inline void set_unoccupied()
      {
        set_value(UNOCCUPIED);
      }
      inline void set_valid()
      {
        set_value(VALID);
      }
      inline void set_missing()
      {
        set_value(MISSING);
      }
      inline void set_disabled()
      {
        set_value(DISABLED);
      }

      inline bool is_unoccupied() const
      {
        return value == UNOCCUPIED;
      }
      inline bool is_valid() const
      {
        return value == VALID;
      }
      inline bool is_missing() const
      {
        return value == MISSING;
      }
      inline bool is_disabled() const
      {
        return value == DISABLED;
      }

      inline operator unsigned short()
      {
        return static_cast<unsigned short>(value);
      }

    private:
      inline void set_value(possible_value val)
      {
        value = val;
      }

      possible_value value;

    };

    class tracking_mode
    {
      enum
      {
        TRANSFORMATION_DATA = 0,  // 0x0001(2^0 )
        TOOL_MARKER_INFOMATION = 1,  // 0x0002(2^1 )
        SINGLE_STRAY_ACTIVE_MARKER = 2,  // 0x0004(2^2 )
        MARKERS_ON_TOOLS = 3,  // 0x0008(2^3 )
        ALL_TRANSFORMATION = 11, // 0x0800(2^11)
        STRAY_PASSIVE_MARKERS = 12, // 0x1000(2^12)
      };

    public:
      tracking_mode()
      {
        default();
      }

      void default()
      {
        mode.reset();
        transformation_data_on();
      }

      operator unsigned short()
      {
        return static_cast<unsigned short>(mode.to_ulong());
      }

      inline void set_transformation_data(bool flag)
      {
        set_flag(TRANSFORMATION_DATA, flag);
      }
      inline void set_tool_marker_information(bool flag)
      {
        set_flag(TOOL_MARKER_INFOMATION, flag);
      }
      inline void set_single_stray_active_marker(bool flag)
      {
        set_flag(SINGLE_STRAY_ACTIVE_MARKER, flag);
      }
      inline void set_markers_on_tool(bool flag)
      {
        set_flag(MARKERS_ON_TOOLS, flag);
      }
      inline void set_all_transformation(bool flag)
      {
        set_flag(ALL_TRANSFORMATION, flag);
      }
      inline void set_stray_passive_markers(bool flag)
      {
        set_flag(STRAY_PASSIVE_MARKERS, flag);
      }

      inline void transformation_data_on()
      {
        set_transformation_data(true);
      }
      inline void tool_marker_information_on()
      {
        set_tool_marker_information(true);
      }
      inline void single_stray_active_marker_on()
      {
        set_single_stray_active_marker(true);
      }
      inline void markers_on_tool_on()
      {
        set_markers_on_tool(true);
      }
      inline void all_transformation_on()
      {
        set_all_transformation(true);
      }
      inline void stray_passive_markers_on()
      {
        set_stray_passive_markers(true);
      }

      inline void transformation_data_off()
      {
        set_transformation_data(false);
      }
      inline void tool_marker_information_off()
      {
        set_tool_marker_information(false);
      }
      inline void single_stray_active_marker_off()
      {
        set_single_stray_active_marker(false);
      }
      inline void markers_on_tool_off()
      {
        set_markers_on_tool(false);
      }
      inline void all_transformation_off()
      {
        set_all_transformation(false);
      }
      inline void stray_passive_markers_off()
      {
        set_stray_passive_markers(false);
      }

      inline bool is_transformation_data_on() const
      {
        return mode[TRANSFORMATION_DATA];
      }
      inline bool is_tool_marker_information_on() const
      {
        return mode[TOOL_MARKER_INFOMATION];
      }
      inline bool is_single_stray_active_marker_on() const
      {
        return mode[SINGLE_STRAY_ACTIVE_MARKER];
      }
      inline bool is_markers_on_tool_on() const
      {
        return mode[MARKERS_ON_TOOLS];
      }
      inline bool is_all_transformation_on() const
      {
        return mode[ALL_TRANSFORMATION];
      }
      inline bool is_stray_passive_markers_on() const
      {
        return mode[STRAY_PASSIVE_MARKERS];
      }

    private:
      inline void set_flag(size_t pos, bool flag)
      {
        mode.set(pos, flag);
      }

      std::bitset<13> mode;
    };

    // reply option 0x0001
    class port_status
    {
    public:
      enum
      {
        OCCUPIED = 0,
        SW1_OR_GPIO1 = 1,
        SW2_OR_GPIO2 = 2,
        SW3_OR_GPIO3 = 3,
        INITIALIZED = 4,
        ENABLED = 5,
        OUT_OF_VOLUME = 6,
        PARTIALLY_OUT_OF_VOLUME = 7,
        ALGORITHM_LIMITATION = 8,
        IR_ITERFERENCE = 9,
        // RESERVED                  = 10,
        // RESERVED                  = 11,
        PROCESSING_EXCEPTION = 12,
        // RESERVED                  = 13,
        FELL_BEHIND_WHILE_PROCESSING = 14,
        DATA_BUFFER_LIMITATION = 15,
        // RESERVED                  = 16 - 31,
      };

      port_status() {}
      explicit port_status(uint32_t value) : stat(value) {}
      uint32_t get_value() const;

      template<int Opt>
      bool is_() const
      {
        return stat[Opt];
      }

    private:
      bool is(uint8_t code) const;
      std::bitset<32> stat;
    };

    struct transform_data
    {
      union
      {
        std::array<float, 8> data;

        struct
        {
          float w, qx, qy, qz, x, y, z, error;
        };

        struct
        {
          std::array<float, 4> quternion;
          std::array<float, 3> translation;
        };
      };

      //std::array<float, 7> data;
      //float error;
      port_status status;
      unsigned int frame;

      transform_data() {}
    };

    // reply option 0x0002
    class tool_information
    {
      enum
      {
        BAD_TRANSFORMATION_FIT = 0,
        NOT_ENOUGH_ACCEPTABLE_MARKERS = 1,
        IR_INTERFERENCE = 2,
        FEEL_BEHIND_WHILE_PROCESSING = 3,
        PROCESSING_EXCEPTION = 7,
      };

    public:
      tool_information() {}
      explicit tool_information(uint8_t value) : tool_info(value) {}

      bool is_bad_transformation_fit() const;
      bool is_not_enough_acceptable_markers() const;
      bool is_IR_interference() const;
      bool is_feel_behind_while_processing() const;
      bool is_processing_exception() const;

      uint8_t get_value() const;

    private:
      std::bitset<8> tool_info;
    };

    class marker_information
    {
      enum
      {
        MISSING = 0x00,
        EXCEEDED_MAX_ANGLE = 0x01,
        EXCEEDED_MAX_ERR = 0x02,
        USED = 0x03,
        USED_OUT_OF_VOLUME = 0x04,
        NOT_USED_OUT_OF_VOLUME = 0x05,
      };

    public:
      marker_information() {}
      marker_information(std::array<uint8_t, 10> const& value)
      {
        for (int i = 0; i < 10; ++i)
        {
          uint8_t const& v = value[i];
          marker_info[2 * i] = v & 0x0F;
          marker_info[2 * i + 1] = (v >> 4);
        }
      }

      bool is_missing(size_t num) const;
      bool is_exceeded_max_angle(size_t num) const;
      bool is_exceeded_max_err(size_t num) const;
      bool is_used(size_t num) const;
      bool is_used_out_of_volume(size_t num) const;
      bool is_not_used_out_of_volume(size_t num) const;

    private:
      std::array<uint8_t, 20> marker_info;
    };

    struct tool_marker_information
    {
      tool_information tool;
      marker_information marker;
    };

    // reply option 0x0004
    class single_active_stray_marker_status
    {
      enum
      {
        VALID = 0,
        MISSING = 1,
        // RESERVED,
        OUT_OF_VOLUME = 3,
      };

    public:
      single_active_stray_marker_status() {}
      single_active_stray_marker_status(uint8_t value) : stat(value) {}

      inline bool is_exist() const
      {
        return !stat.none();
      }

      inline bool is_valid() const
      {
        return stat[VALID];
      }
      inline bool is_missing() const
      {
        return stat[MISSING];
      }
      inline bool is_out_of_volume() const
      {
        return stat[OUT_OF_VOLUME];
      }

    private:
      std::bitset<8> stat;
    };

    struct single_active_stray_marker
    {
      single_active_stray_marker_status status;
      std::array<float, 3> position;
    };

    // reply option 0x0008 and 0x1000
    struct marker_position
    {
      uint16_t num_marker;
      boost::dynamic_bitset<uint8_t> out_of_volume;
      std::vector<std::array<float, 3>> positions;
    };

    // handle data (0x0001 - 0x0008)
    struct handle_data
    {
      tracking_mode mode;
      handle_status status;
      transform_data transform_data;

      // tracking_mode = 0x0002
      tool_marker_information tool_marker_information;

      // tracking_mode = 0x0004
      single_active_stray_marker single_active_stray_marker;

      // tracking_mode = 0x0008
      marker_position tool_markers;

    };

    struct tracking_data
    {
      std::map<uint16_t, handle_data> tools;

      // 0x1000
      marker_position stray_markers;
    };
  }
}


#endif
