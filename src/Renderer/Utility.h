#pragma once
#include <fstream>
#include <vector>
#include <filesystem>

static std::vector<char> LoadCSO( const std::string& filename )
{
	std::ifstream ifs( filename, std::ios::binary | std::ios::ate );
	if( !ifs.is_open() )
	{
		std::string errorMsg = "Failed to load shader: " + filename;
		errorMsg += "\nCurrent working directory: " + std::filesystem::current_path().string();
		throw std::runtime_error( errorMsg );
	}
	std::streamsize size = ifs.tellg();
	if( size == 0 )
	{
		throw std::runtime_error( "Shader file is empty: " + filename );
	}
	ifs.seekg( 0, std::ios::beg );

	std::vector<char> buffer( size );
	ifs.read( buffer.data(), size );
	if( !ifs )
	{
		throw std::runtime_error( "Failed to read shader file: " + filename );
	}
	return buffer;
}