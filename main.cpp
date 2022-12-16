#include "DOA6/G1mFile.h"
#include "debug.h"

static std::string get_path_from_input()
{
	static char str[MAX_PATH];
	gets(str);
	
	std::string path = str;
	Utils::TrimString(path);
	
	if (path.length() > 0)
	{
		if (path[0] == '"')
			path = path.substr(1);
		
		if (path.length() > 0 && path.back() == '"')
			path.pop_back();
	}
	
	return path;
}

bool hide_mesh(const std::string &path)
{
	G1mFile g1m;
	
	if (!Utils::EndsWith(path, ".g1m", false))
	{
		DPRINTF("Error: File should have .g1m extension.\n");
		return false;
	}
	
	g1m.SetParseNun(false);
	
	if (!g1m.LoadFromFile(path.c_str()))
		return false;
	
	size_t num_submeshes = g1m.GetNumSubmeshes();
	
	if (num_submeshes == 0)
	{
		DPRINTF("This g1m file has no submeshes.\n");
		return false;
	}
	
	UPRINTF("This g1m has %Id submeshes (index 0-%Id)\n", num_submeshes, num_submeshes-1);
	UPRINTF("Enter the index of the mesh(es) to remove and press enter.\nYou can input multiple indexes separated by comma.\n");
	
	static char str[256];
	gets(str);
	
	std::vector<std::string> str_list;
	std::vector<uint32_t> list;
	if (Utils::GetMultipleStrings(str, str_list) == 0)
	{
		DPRINTF("Nothing was in the input!\n");
		return false;
	}
		
	for (std::string &num_str : str_list)
	{
		Utils::TrimString(num_str);
		
		if (!Utils::HasOnlyDigits(num_str))
		{
			DPRINTF("%s is not a valid number.\n", num_str.c_str());
			return false;
		}
		
		uint32_t num = Utils::GetUnsigned(num_str);
		if (num >= (uint32_t)num_submeshes)
		{
			DPRINTF("%d is out of range.\n", num);
			return false;
		}
		
		list.push_back(num);
	}
	
	for (uint32_t idx : list)
	{
		UPRINTF("Hiding submesh %u...\n", idx);
		
		if (!g1m.HideSubmesh(idx))
		{
			DPRINTF("Failed to hide submesh %u\n", idx);
			return false;
		}
	}
	
	
	if (!g1m.SaveToFile(path))
		return false;
	
	UPRINTF("Success!\n");
	return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        DPRINTF("Bad usage. Usage: %s file\n", argv[0]);
		UPRINTF("Press enter to exit.");
		getchar();
        return -1;
    }

    int ret = hide_mesh(Utils::NormalizePath(argv[1]));

	fseek(stdin, 0, SEEK_END);
	UPRINTF("Press enter to exit.");
    getchar();

    return ret;
}
