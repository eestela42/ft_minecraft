#include <classes/Generation/ChunkGenerator.hpp>


std::vector<siv::PerlinNoise*> 		ChunkGenerator::noiseList;
std::map<position, std::vector<u_char>*> ChunkGenerator::modifMap;
int ChunkGenerator::seed;

#define GEN_NOISE3D true
#define GEN_WATER true

ChunkGenerator::~ChunkGenerator()
{
	
}

void ChunkGenerator::initNoise(u_int seed)
{
	ChunkGenerator::seed = seed;
	

	siv::PerlinNoise *noise0 = new siv::PerlinNoise(seed);
	siv::PerlinNoise *noise1 = new siv::PerlinNoise(seed + 13);

	siv::PerlinNoise *noise2 = new siv::PerlinNoise(seed + 59);
	siv::PerlinNoise *noise3 = new siv::PerlinNoise(seed + 42);
	siv::PerlinNoise *noise4 = new siv::PerlinNoise(seed + 53);
	siv::PerlinNoise *noise5 = new siv::PerlinNoise(seed + 23);
	siv::PerlinNoise *noise6 = new siv::PerlinNoise(seed + 17);
	siv::PerlinNoise *noise7 = new siv::PerlinNoise(seed + 19);

	ChunkGenerator::noiseList.push_back(noise0);
	ChunkGenerator::noiseList.push_back(noise1);
	ChunkGenerator::noiseList.push_back(noise2);
	ChunkGenerator::noiseList.push_back(noise3);
	ChunkGenerator::noiseList.push_back(noise4);
	ChunkGenerator::noiseList.push_back(noise5);
	ChunkGenerator::noiseList.push_back(noise6);
	ChunkGenerator::noiseList.push_back(noise7);
}

ChunkGenerator::ChunkGenerator()
{
	ASysteme *base = new SystemeCall();
}

ChunkGenerator::ChunkGenerator(__attribute__((unused)) u_int seed)
{
	
}

void ChunkGenerator::generateTree(int x, int y, int z) {
	int tronc = z - 1;
	for (; tronc < z + 4; tronc++) {
		data[x * AChunk::sizeZ + tronc + y * AChunk::sizeY * AChunk::sizeZ] = OAK_WOOD_SIDE;
	}
	for (int i = -1; i < 2; i++) {
	for (int j = -1; j < 2; j++) {
	for (int k = 0; k < 2; k++) {
		if (x+i >= 0 && x+i <= AChunk::sizeX - 1 && y+j >= 0 && y+j <= AChunk::sizeY - 1 && tronc+k >= 0 && tronc+k < AChunk::sizeZ)
			data[(x+i) * AChunk::sizeZ + (tronc+k) + (y+j) * AChunk::sizeY * AChunk::sizeZ] = LEAVES_2;
	}
	}
	}
	if (tronc + 2 < AChunk::sizeZ)
		data[x * AChunk::sizeZ + tronc + 2 + y * AChunk::sizeY * AChunk::sizeZ] = LEAVES_2;
	
}

int ChunkGenerator::genBedrock(u_char *data, int x, int y)
{
	
	double bedrock_factor = 0;
	int bedHigh = 3 + (int)(bedrock_factor * 5);
	int k = 0;
	data[x * sizeZ + y * sizeX * sizeZ] = BEDROCK;
	for (; k < bedHigh; k++)
	{
		data[x * sizeZ + y * sizeX * sizeZ + k] = BEDROCK;
	}
	return k;
}

int ChunkGenerator::genUnderLayer(int pos, int &z)
{
		double ground_factor = noiseList[0]->octave2D_01(0.002456 * p_x, 0.001495 * p_y, 1, 0.67);
	


		if (ground_factor > 0.5)
			ground_factor *= 1.0f + (ground_factor - 0.5f) * 4 * ground_factor;

		ground_height = (int)(ground_factor * 90);


		bool isWater = false;
		if (ground_factor < 0.25)
		{
			isWater = true;
		}

		while (z < ground_height) {
			data[pos + z] = STONE;
			z++;
		}


		double hill_factor = noiseList[1]->octave2D_01(0.00628 * p_x, 0.006368 * p_y, 1, 0.5);
		++hill_factor /= 2;


		if (hill_factor > 0.5)
			hill_factor *= 1.0f + (hill_factor - 0.5f) * 4 * hill_factor;

		hill_height = ground_height + (int)((hill_factor * ground_factor) * 90 + 5);
		

		if (ground_factor + hill_factor  < 1.15)
		{
			while (z < hill_height) {
				data[pos + z] = STONE;
				z++;
			}
		}
		else
		{
			int air_end = 0;
			for ( ; z < hill_height && z < AChunk::sizeZ; z++)
			{
				double montain_factor = noiseList[3]->octave3D(0.0256 * p_x, 0.0295 * p_y, z * 0.023, 1, 0.56);
				if (montain_factor < float(z)/(hill_height*2))
				{
					air_end++;
					data[pos + z] = AIR;
					continue ;
				}
				air_end = 0;
				data[pos + z] = STONE;
			}
			z -= air_end;
			hill_height = z;
		}

	return 0;
}

int ChunkGenerator::genOverLayer( int pos, int &z)
{
	double detail_factor =  noiseList[2]->octave2D_01(0.206 * p_x, 0.204 * p_y, 1, 0.4);
	int detail_height = hill_height + detail_factor * 6;
	
	while (z < detail_height) {
		data[pos + z] = STONE;
		z++;
	}



	double dirt_factor = noiseList[2]->octave2D_01(0.00156 * p_x, 0.00195 * p_y, 1, 0.5);
	int dirt_height = hill_height + (int)((dirt_factor) * 30);

	u_char under_layer = DIRT;
	u_char over_layer = GRASS;

	double geologie = noiseList[5]->octave2D_01(0.0002887 * p_x, 0.0002689 * p_y, 1, 0.7);
	if (geologie > 0.6)
	{
		under_layer = SAND;
		over_layer = SAND;
	}

	if (hill_height > 80 + (geologie * 180))
		over_layer = SNOW_BLOCK;
	
	
	while (z < dirt_height && z < AChunk::sizeZ) {
		data[pos + z] = under_layer;
		z++;
	}


	if (z > 60 && z == dirt_height && dirt_factor > 0.1)
	{
		
		data[pos + z -1] = over_layer;
		if (geologie < 0.6 && over_layer == GRASS)
		{
			double tree_factor = noiseList[3]->octave2D_01(0.00856 * p_x, 0.00665 * p_y, 1, 0.4);
			tree_factor /= 10;
			if ((engine.operator()() % 1000) / tree_factor < 1)
			{
				int x = pos / sizeZ % sizeX;
				int y = pos / (sizeZ * sizeX);
				if (x > 2 && x < 13 && y > 2 && y < 13)
					generateTree(x, y, z);
			}
		}
	}
	return 0;
}

int ChunkGenerator::gen2DCave(int hill_height, int pos, int &z)
{
	double tunel = noiseList[5]->octave2D_01(0.0076 * p_x, 0.0065 * p_y, 1, 0.3);
		if ((tunel > 0.33 && tunel < 0.36) || (tunel > 0.64 && tunel < 0.67) || (tunel > 0.94 && tunel < 0.97))
		{
			int start = noiseList[6]->octave2D_01(0.0056 * p_x, 0.0045 * p_y, 1, 0.3) * (hill_height - 8);
			int size = noiseList[7]->octave2D_01(0.0126 * p_x, 0.0135 * p_y, 1, 0.5) * 9;
			for (z = start; z < start + size; z++)
			{
				data[pos + z] = AIR;
			}
		}
	tunel = noiseList[6]->octave2D_01(0.0076 * p_x, 0.0065 * p_y, 1, 0.3);
		if ((tunel > 0.33 && tunel < 0.36) || (tunel > 0.64 && tunel < 0.67) || (tunel > 0.94 && tunel < 0.97))
		{
			int start = noiseList[7]->octave2D_01(0.0056 * p_x, 0.0045 * p_y, 1, 0.3) * (hill_height - 8);
			int size = noiseList[5]->octave2D_01(0.0126 * p_x, 0.0135 * p_y, 1, 0.5) * 9;
			for (z = start; z < start + size; z++)
			{
				data[pos + z] = AIR;
			}
		}
	return 0;
}

int ChunkGenerator::genWater( int pos, int level)
{
	
	while (!data[pos + level])
	{
		data[pos + level] = WATER;
		level--;
	}
	return 0;
}

int ChunkGenerator::gen3DCave(int hill_height, int pos, int &z)
{
	z = 1;
	for ( ; z < hill_height + 2; z++)
	{
		if (z < 32)
		{
			double diamond_factor = noiseList[0]->octave3D(0.0156 * p_x, 0.095 * p_y, z * 0.39, 1, 0.5);
			if ((diamond_factor > 0.90  || diamond_factor < 0.1)) {
				data[pos + z] = DIAMOUND_MINERAL;
				continue ;
			}
		}
		if (z < 64)
		{
			double iron_factor = noiseList[1]->octave3D(0.0156 * p_x, 0.0195 * p_y, z * 0.49, 1, 0.5);
			if ((iron_factor > 0.49  && iron_factor < 0.51)) {
				data[pos + z] = IRON_MINERAL;
				continue ;
			}
		}
	}

	z = 1;
	for ( ; z < hill_height + 2; z++)
	{
		double cave_factor = noiseList[3]->octave3D(0.01556 * p_x, 0.01595 * p_y, z * 0.06, 1, 0.5);
		if (cave_factor > 0.8 || cave_factor < 0.2) {
			data[pos + z] = AIR;
			continue ;
		}
		// double spag_factor = noiseList[0]->octave3D(0.0256 * p_x, 0.0295 * p_y, z * 0.039, 1, 0.5);
		// if ((spag_factor > 0.41  && spag_factor < 0.44)) {
		// 	data[pos + z] = AIR;
		// 	continue ;
		// }
		
	}
	return 0;
}

#include <spline.h>

float easeInOutSine(float x)
{
	return -(glm::cos(M_PI * x) - 1) / 2;
}
float easeInOutBounce(float x)
{
	if (x < 0.5)
		return (1 - easeInOutSine(1 - 2 * x)) / 2;
	return (1 + easeInOutSine(2 * x - 1)) / 2;
}

double limiteExponentielle(double x, double L, double k) {
    return L * (1 - std::exp(-k * x));
}

int ChunkGenerator::newGenUnderLayer(int pos, int &z, param p)
{
	tk::spline	spline;

	spline.set_points(	{0.0, 	0.3,	0.4,	0.7,	0.75,	1},
						{1,		10,		50,		70,		80,		130});
	u_int ground_height = spline(p.geologie);

	while (z <  ground_height && z < AChunk::sizeZ - 2)
		data[pos + z++] = STONE;

	double plane_factor = noiseList[1]->octave2D_01((0.000128 ) * p_x, (0.0001368 ) * p_y, 1, 0.75);
	u_int type = GRASS;
	double lerp = plane_factor;
	if (p.geologie < 0.4)
	{
		double ocean_factor = noiseList[7]->octave2D_01((0.000128 ) * p_x, (0.0001368 ) * p_y, 1, 0.75);

		spline.set_points(	{0.0, 0.2, 0.4},
							{1.0, 0.6, 0.0});
		lerp = glm::mix(plane_factor, ocean_factor, spline(p.geologie));
		type = SAND;	
	}
	else if (p.geologie >= 0.7)
	{
		double mont_freq =  0.00628;
		double mont_freq_2 =  0.0000328;
		double montain_factor =  noiseList[2]->octave2D_01(mont_freq * p_x, mont_freq * p_y, 4, 0.75);
		double montain_factor2 = noiseList[3]->octave2D_01(mont_freq_2 * p_x, mont_freq_2 * p_y, 4);
		montain_factor *= montain_factor;
		montain_factor += montain_factor2;
		spline.set_points(	{0.0,	 0.9,	4},
							{0.0,	 0.9,	1});
		montain_factor = spline(montain_factor);
		// montain_factor = limiteExponentielle(montain_factor, 1, 0.01);

		spline.set_points(	{0.0,	 0.7,	1},
							{0.0,	 0.0,	1});
		lerp = glm::mix(plane_factor, montain_factor, spline(p.geologie));
		type = STONE;
	}
  
	spline.set_points(	{0.0, 	0.3,	0.4,	0.7,	0.75,	1},
						{1,		10,		50,		70,		80,		130});
					
	int max_height = spline(p.geologie);
	// int max_height = limiteExponentielle(p.geologie, 255, 0.05);

	int hill_height = z + 1 + lerp * max_height ;


	while (z <  hill_height && z < AChunk::sizeZ - 2) {
		data[pos + z] = type;
		z++;
	}
	
	
	// return 0;

	
	// tk::spline	spline_geo;
	// 	spline_geo.set_points(	{0.0,	 0.35, 	0.4,	0.7,	0.8,	0.9,	1},
	// 						{30,	 45,	75,		100,	190,	175,	250});

	// tk::spline	spline_piq;
	// 	spline_piq.set_points(	{0.0,	 0.5, 	0.6,	0.7,	0.8,	0.9,	1},
	// 							{0,	 	50,		40,		75,		160,	210,	250});

	// tk::spline spline_ero;
	// spline_ero.set_points(	{0.0,	 0.35, 	0.4,	0.7,	0.8,	0.9,	1},
	// 						{250,	 120,	100,	50,		30,		35,		10});

	// int terrain_height = (spline_geo(p.geologie) + spline_piq(p.pique) + spline_ero(p.erosion)) / 3;

	// int base_height = 100;
	// int terrain_height = spline(terrain_factor);

	// while (z <  terrain_height && z < AChunk::sizeZ - 2) {
	// 	data[pos + z] = STONE;
	// 	z++;
	// }
	


}

u_char		*ChunkGenerator::generator(glm::ivec2 tmp_pos)
{	
	sizeX = AChunk::sizeX;
	sizeY = AChunk::sizeY;
	sizeZ = AChunk::sizeZ;
	posX = tmp_pos.x;
	posY = tmp_pos.y;

	data = (u_char*)calloc(sizeX * sizeY * sizeZ, sizeof(*data));
	if (!data)
		std::cout << "raw map calloc failed !" << std::endl;

	for (int y = 0; y < sizeY; y++){
	for (int x = 0; x < sizeX; x++)
	{
		
	}

	}




	// engine.seed(seed * posX * posY);
	
	// for (int y = 0; y < sizeY; y++)
	// {
	// 	for (int x = 0; x < sizeX; x++)
	// 	{
	// 		ground_height = 0;
	// 		hill_height = 0;
			
	// 		int pos = x * sizeZ + y * sizeX * sizeZ;
	// 		p_x = ((double)posX * sizeX + x);
	// 		p_y = ((double)posY * sizeY + y);
			
	// 		int z = genBedrock(data, x, y);

	// 		param p;

	// 		p.geologie = noiseList[4]->octave2D_01(0.0028327 * p_x, 0.002479 * p_y, 4, 1);
	// 		p.pique = noiseList[5]->octave2D_01(0.0045327 * p_x, 0.004679 * p_y, 4, 1);
	// 		p.erosion = noiseList[3]->octave2D_01(0.0048327 * p_x, 0.004479 * p_y, 4, 1);

	// 		newGenUnderLayer(pos, z, p);
	// 		genWater(pos, 65);
	// 	}
	// }
	return data;
}




{
	ocean:{
		high:{
			x : 0, y : 0
		}
	}
}