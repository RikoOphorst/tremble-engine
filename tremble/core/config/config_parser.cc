#include "config_parser.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	ConfigParser::ConfigParser()
	{
	}
	
	//------------------------------------------------------------------------------------------------------
	ConfigParser::~ConfigParser()
	{
	}
	
	//------------------------------------------------------------------------------------------------------
	Config ConfigParser::Parse(const std::string& json_config)
	{
		Config ret;

		std::ifstream myfile;
		myfile.open(json_config);

		std::string contents;
		std::string line;
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				contents += line;
				contents += "\n";
			}

			myfile.close();
		}

		picojson::value v;
		std::string err = picojson::parse(v, contents);

		if (!v.is<picojson::object>())
		{
			std::cerr << "Config file corrupted. Using default." << std::endl;
			return ret;
		}

		const picojson::value::object& obj = v.get<picojson::object>();
		
		ret.fullscreen			= obj.find("fullscreen")			!= obj.end() ? obj.at("fullscreen").get<bool>()									: false;
		ret.adapter				= obj.find("adapter")				!= obj.end() ? static_cast<UINT>(obj.at("adapter").get<int64_t>())				: 0;
		ret.render_resolution	= obj.find("render_resolution")		!= obj.end() ? static_cast<UINT>(obj.at("render_resolution").get<int64_t>())	: 0;
		ret.window_resolution	= obj.find("window_resolution")		!= obj.end() ? static_cast<UINT>(obj.at("window_resolution").get<int64_t>())	: 0;
		ret.scene				= obj.find("scene")					!= obj.end() ? static_cast<UINT>(obj.at("scene").get<int64_t>())				: 0;
		ret.load_scene			= obj.find("load_scene")			!= obj.end() ? obj.at("load_scene").get<bool>()									: false;
		ret.god_mode			= obj.find("god_mode")				!= obj.end() ? obj.at("god_mode").get<bool>()									: false;
		ret.frustum_culling		= obj.find("frustum_culling")		!= obj.end() ? obj.at("frustum_culling").get<bool>()							: false;
		ret.depth_pre_pass		= obj.find("depth_pre_pass")		!= obj.end() ? obj.at("depth_pre_pass").get<bool>()								: false;
		ret.physics_base_plane	= obj.find("physics_base_plane")	!= obj.end() ? obj.at("physics_base_plane").get<bool>()							: false;
		ret.d3d12_debug_layer	= obj.find("d3d12_debug_layer")		!= obj.end() ? obj.at("d3d12_debug_layer").get<bool>()							: false;
		ret.wireframe_rendering = obj.find("wireframe_rendering")	!= obj.end() ? obj.at("wireframe_rendering").get<bool>()						: false;
		ret.light_demo			= obj.find("light_demo")			!= obj.end() ? obj.at("light_demo").get<bool>()									: false;
		ret.num_lights			= obj.find("num_lights")			!= obj.end() ? static_cast<UINT>(obj.at("num_lights").get<int64_t>())			: 0;
		ret.master_volume		= obj.find("master_volume")			!= obj.end() ? static_cast<UINT>(obj.at("master_volume").get<int64_t>())		: 0;
		ret.mouse_sensitivity	= obj.find("mouse_sensitivity")		!= obj.end() ? static_cast<UINT>(obj.at("mouse_sensitivity").get<int64_t>())	: 0;
		ret.host				= obj.find("host")					!= obj.end() ? obj.at("host").get<bool>()										: false;
		ret.host_ip_address		= obj.find("host_ip_address")		!= obj.end() ? obj.at("host_ip_address").get<std::string>()						: 0;
		ret.nickname			= obj.find("nickname")				!= obj.end() ? obj.at("nickname").get<std::string>()							: 0;
		ret.spawn_clients		= obj.find("spawn_clients")			!= obj.end() ? obj.at("spawn_clients").get<bool>()								: false;
		ret.num_clients			= obj.find("num_clients")			!= obj.end() ? static_cast<UINT>(obj.at("num_clients").get<int64_t>())			: 0;

		return ret;
	}
	
	//------------------------------------------------------------------------------------------------------
	void ConfigParser::Serialize(const std::string& config_path, Config& config)
	{
		std::initializer_list<std::pair<const std::string, picojson::value>> list = 
		{
			std::pair<std::string, picojson::value>("fullscreen", picojson::value(config.fullscreen)),
			std::pair<std::string, picojson::value>("adapter", picojson::value(static_cast<double>(config.adapter))),
			std::pair<std::string, picojson::value>("render_resolution", picojson::value(static_cast<double>(config.render_resolution))),
			std::pair<std::string, picojson::value>("window_resolution", picojson::value(static_cast<double>(config.window_resolution))),
			std::pair<std::string, picojson::value>("scene", picojson::value(static_cast<double>(config.scene))),
			std::pair<std::string, picojson::value>("load_scene", picojson::value(config.load_scene)),
			std::pair<std::string, picojson::value>("god_mode", picojson::value(config.god_mode)),
			std::pair<std::string, picojson::value>("frustum_culling", picojson::value(config.frustum_culling)),
			std::pair<std::string, picojson::value>("depth_pre_pass", picojson::value(config.depth_pre_pass)),
			std::pair<std::string, picojson::value>("physics_base_plane", picojson::value(config.physics_base_plane)),
			std::pair<std::string, picojson::value>("d3d12_debug_layer", picojson::value(config.d3d12_debug_layer)),
			std::pair<std::string, picojson::value>("wireframe_rendering", picojson::value(config.wireframe_rendering)),
			std::pair<std::string, picojson::value>("light_demo", picojson::value(config.light_demo)),
			std::pair<std::string, picojson::value>("num_lights", picojson::value(static_cast<double>(config.num_lights))),
			std::pair<std::string, picojson::value>("master_volume", picojson::value(static_cast<double>(config.master_volume))),
			std::pair<std::string, picojson::value>("mouse_sensitivity", picojson::value(static_cast<double>(config.mouse_sensitivity))),
			std::pair<std::string, picojson::value>("host", picojson::value(config.host)),
			std::pair<std::string, picojson::value>("host_ip_address", picojson::value(config.host_ip_address)),
			std::pair<std::string, picojson::value>("nickname", picojson::value(config.nickname)),
			std::pair<std::string, picojson::value>("spawn_clients", picojson::value(config.spawn_clients)),
			std::pair<std::string, picojson::value>("num_clients", picojson::value(static_cast<double>(config.num_clients)))
		};

		picojson::value v = picojson::value(picojson::object(list));

		std::string output = v.serialize(true);

		std::ofstream file(config_path);

		if (file.is_open())
		{
			file << output;
			file.close();
		}
	}
}