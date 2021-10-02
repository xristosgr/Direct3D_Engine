#include "HeightfieldTerrain.h"


float HeightfieldTerrain::WavesHeightCalculator(int& i)
{
	//float x = 5.7f;
	//float y = 2.5f;
	//HeightData[i] = (waveHeight)*sin(( waveDistance) * (dotProduct(randomOffsetsX[i], randomOffsetsY[i],x,y)+ waveSpeed + i+ randomOffsetsY[i]));
	////OutputDebugStringA(("Y[" + std::to_string(i) + "] = " + std::to_string(HeightData[i])+"\n").c_str());
	//return HeightData[i];
	return 0.0f;
}
float HeightfieldTerrain::WavesHorizontalCalculator(int& i)
{
	//float x = 0.7f;
	//float y = 1.5f;
	//HorizontalData[i] =   sin((waveDistance) * (dotProduct(randomOffsetsX[i], randomOffsetsY[i], x, y) + waveSpeed + (float)i+ randomOffsetsX[i]));
	//terrainVals[i] = sin((waveDistanceH) * (waveSpeedH + i));


	return 0.0f;
}
float HeightfieldTerrain::WavesVerticalCalculator(int& i)
{
	float z = 3.7f;
	float y = 0.5f;
	//HorizontalData[i] =  randomOffsetsZ[i] +  sin((waveDistance) * (dotProduct(randomOffsetsX[i], randomOffsetsY[i], z, y) + waveSpeed + (float)i+ randomOffsetsZ[i]));
	//terrainVals[i] = sin((waveDistanceV) * (waveSpeedV + i));

	return 0.0f;
}
float HeightfieldTerrain::dotProduct(float& x1, float& y1, float& x2, float& y2)
{
	return x1*x2 + y1*y2;
}
double HeightfieldTerrain::Drand(double min, double max)
{
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<> dist(min, max);


	return dist(e2);
}

bool HeightfieldTerrain::LoadSetupFile(const char* filename)
{
	int stringLength;
	std::ifstream fin;
	char input;

	stringLength = 256;
	//terrainFileName = "Data/Heightmaps/Heightmap.bmp";
	m_terrainFilename = new char[stringLength];
	if(!m_terrainFilename)
	{
		OutputDebugStringA("Invalid file path!\n");
		return false;
	}
	//m_colorMapFilename = new char[stringLength];
	//if (!m_colorMapFilename)
	//{
	//	OutputDebugStringA("Invalid file path!\n");
	//	return false;
	//}

	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_terrainFilename;
	
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_terrainHeight;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_terrainWidth;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin >> m_heightScale;

	//fin.get(input);
	//while (input != ':')
	//{
	//	fin.get(input);
	//}
	//fin >> m_colorMapFilename;


	fin.close();
	

	OutputDebugStringA(m_colorMapFilename);
	OutputDebugStringA("\n");
	return true;
}

bool HeightfieldTerrain::LoadRawHeightMap()
{
	int error, i, j, index;
	FILE* fileptr;
	unsigned long long imageSize, count;
	unsigned short* rawImage;

	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		OutputDebugStringA("Error raw 1!\n");
		return false;
	}
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0)
	{
		return false;
	}
	imageSize = m_terrainHeight * m_terrainWidth;
	rawImage = new unsigned short[imageSize];
	if (!rawImage)
	{
		OutputDebugStringA("Error raw 2!\n");
		return false;
	}
	count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	//if (count != imageSize)
	//{
	//	OutputDebugStringA("Error raw 3!\n");
	//	return false;
	//}
	error = fclose(filePtr);
	if (error != 0)
	{
		OutputDebugStringA("Error raw 4!\n");
		return false;
	}

	for (j = 0; j < m_terrainHeight; ++j)
	{
		for (i = 0; i < m_terrainWidth; ++i)
		{
			index = (m_terrainWidth * j) + i;
			m_heightMap[index].y = (float)rawImage[index];
		}
	}

	return true;
}

bool HeightfieldTerrain::LoadBitMapHeightMap()
{

	int error, imageSize, i, j, k, index;
	
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;


	// Start by creating the array structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		OutputDebugStringA("ERROR1!\n");
		return false;
	}

	// Open the bitmap map file in binary.
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0)
	{
		OutputDebugStringA("ERROR2!\n");
		return false;
	}

	// Read in the bitmap file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		OutputDebugStringA("ERROR3!\n");
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		OutputDebugStringA("ERROR4!\n");
		return false;
	}

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	//if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
	//{
	//	OutputDebugStringA("ERROR5!\n");
	//	return false;
	//}

	// Calculate the size of the bitmap image data.  
	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.

	if ((m_terrainWidth * m_terrainHeight) % 2 == 0)
	{
		imageSize = m_terrainHeight * ((m_terrainWidth * 3));
	}
	else
	{
		imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);
	}


	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		OutputDebugStringA("ERROR6!\n");
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		OutputDebugStringA("ERROR7!\n");
		return false;
	}

	// Close the file.
	//error = fclose(filePtr);
	//if (error != 0)
	//{
	//	OutputDebugStringA("ERROR8!\n");
	//	return false;
	//}

	// Initialize the position in the image data buffer.
	k = 0;
	
	// Read the image data into the height map array.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			m_heightMap[index].y = (float)height;
			
		
			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data now that the height map array has been loaded.
	delete[] bitmapImage;
	bitmapImage = 0;
	//
	//// Release the terrain filename now that is has been read in.
	delete[] m_terrainFilename;
	m_terrainFilename = 0;

	return true;
}
void HeightfieldTerrain::SetTerrainCoordinates()
{
	int i, j, index;


	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			// Set the X and Z coordinates.
			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			m_heightMap[index].z += (float)(m_terrainHeight - 1);

			// Scale the height.
			m_heightMap[index].y /= m_heightScale;
			//m_heightMap[index].x *= 50.0;
			//m_heightMap[index].z *= 50.0;
		}
	}

	return;
}
bool HeightfieldTerrain::LoadColorMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	// Open the color map file in binary.
	error = fopen_s(&filePtr, m_colorMapFilename, "rb");
	if (error != 0)
	{
		OutputDebugStringA("Error_Color_1!\n");
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		OutputDebugStringA("Error_Color_2!\n");
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		OutputDebugStringA("Error_Color_3!\n");
		return false;
	}

	// Make sure the color map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	//if ((bitmapInfoHeader.biWidth != m_terrainWidth) || (bitmapInfoHeader.biHeight != m_terrainHeight))
	//{
	//	OutputDebugStringA("Error_Color_4!\n");
	//	return false;
	//}


	if ((m_terrainWidth * m_terrainHeight) % 2 == 0)
	{
		imageSize = m_terrainHeight * ((m_terrainWidth * 3));
	}
	else
	{
		imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);
	}

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		OutputDebugStringA("Error_Color_5!\n");
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		OutputDebugStringA("Error_Color_6!\n");
		//return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		OutputDebugStringA("Error_Color_7!\n");
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the color map portion of the height map structure.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			m_heightMap[index].b = (float)bitmapImage[k] / 255.0f;
			m_heightMap[index].g = (float)bitmapImage[k + 1] / 255.0f;
			m_heightMap[index].r = (float)bitmapImage[k + 2] / 255.0f;
			

			k += 3;
		}

		// Compensate for extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the color map filename now that is has been read in.
	delete[] m_colorMapFilename;
	m_colorMapFilename = 0;


	return true;
}
bool HeightfieldTerrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;
	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	for (j = 0; j < m_terrainHeight - 1; j++)
	{
		for (i = 0; i < m_terrainWidth - 1; i++)
		{
			index1 = ((j + 1) * m_terrainWidth) + i;      // Bottom left vertex.
			index2 = ((j + 1) * m_terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * m_terrainWidth) + i;          // Upper left vertex.

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainWidth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length.
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// Normalize the final value for this face using the length.
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}

	}
	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}
	// Release the temporary normals.
	delete[] normals;
	normals = 0;
	return true;
}

bool HeightfieldTerrain::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;
	float quadsCovered, incrementSize, tu2Left, tu2Right, tv2Bottom, tv2Top;

	// Calculate the number of vertices in the 3D terrain model.
	m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	// Create the 3D terrain model array.
	m_terrainModel = new ModelType[m_vertexCount];

	if (!m_terrainModel)
	{
		return false;
	}

	quadsCovered = 32.0f;
	//incrementSize = 1.0f / quadsCovered;
	incrementSize = 1.0f / 128.0f;
	tu2Left = 0.0f;
	tu2Right = incrementSize;
	//tv2Top = 0.0f;
	tv2Bottom = 1.0f;
	tv2Top = 1.0f - incrementSize;

	// Initialize the index into the height map array.
	index = 0;
	this->vertex1 = new XMFLOAT3[m_vertexCount];
	this->vertex2 = new XMFLOAT3[m_vertexCount];
	this->vertex3 = new XMFLOAT3[m_vertexCount];
	this->vertex4 = new XMFLOAT3[m_vertexCount];
	this->vertex5 = new XMFLOAT3[m_vertexCount];
	this->vertex6 = new XMFLOAT3[m_vertexCount];

	this->index1 = new UINT[m_vertexCount];
	this->index2 = new UINT[m_vertexCount];
	this->index3 = new UINT[m_vertexCount];
	this->index4 = new UINT[m_vertexCount];
	// Load the 3D terrain model with the height map terrain data.
	// We will be creating 2 triangles for each of the four points in a quad.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (m_terrainWidth * j) + i;          // Upper left.
			index2 = (m_terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (m_terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.
			this->index1[index] = index1;
			this->index2[index] = index2;
			this->index3[index] = index3;
			this->index4[index] = index4;

			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.
			m_terrainModel[index].x = m_heightMap[index1].x;
			m_terrainModel[index].y = m_heightMap[index1].y;
			m_terrainModel[index].z = m_heightMap[index1].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index1].nx;
			m_terrainModel[index].ny = m_heightMap[index1].ny;
			m_terrainModel[index].nz = m_heightMap[index1].nz;
			m_terrainModel[index].r = m_heightMap[index1].r;
			m_terrainModel[index].g = m_heightMap[index1].g;
			m_terrainModel[index].b = m_heightMap[index1].b;

			m_terrainModel[index].tu2 = tu2Left;
			m_terrainModel[index].tv2 = tv2Top;

	
			index++;

			// Triangle 1 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			m_terrainModel[index].tu2 = tu2Right;
			m_terrainModel[index].tv2 = tv2Top;



			index++;

			// Triangle 1 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			m_terrainModel[index].tu2 = tu2Left;
			m_terrainModel[index].tv2 = tv2Bottom;



			index++;

			// Triangle 2 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			m_terrainModel[index].tu2 = tu2Left;
			m_terrainModel[index].tv2 = tv2Bottom;

	

			index++;

			// Triangle 2 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			m_terrainModel[index].tu2 = tu2Right;
			m_terrainModel[index].tv2 = tv2Top;



			index++;

			// Triangle 2 - Bottom right.
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index4].nx;
			m_terrainModel[index].ny = m_heightMap[index4].ny;
			m_terrainModel[index].nz = m_heightMap[index4].nz;
			m_terrainModel[index].r = m_heightMap[index4].r;
			m_terrainModel[index].g = m_heightMap[index4].g;
			m_terrainModel[index].b = m_heightMap[index4].b;
			m_terrainModel[index].tu2 = tu2Right;
			m_terrainModel[index].tv2 = tv2Bottom;


			index++;

			tu2Left += incrementSize;
			tu2Right += incrementSize;
			//if (tu2Right > 1.0f)
			//{
			//	tu2Left = 0.0f;
			//	tu2Right = incrementSize;
			//}
			//if (tv2Bottom > 1.0f)
			//{
			//	tv2Top = 0.0f;
			//	tv2Bottom = incrementSize;
			//}
		}
		tu2Left = 0.0f;
		tu2Right = incrementSize;

		tv2Top -= incrementSize;
		tv2Bottom -= incrementSize;
	}
	delete[] m_heightMap;
	m_heightMap = 0;

	return true;
}

void HeightfieldTerrain::CalculateTerrainVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal;


	// Calculate the number of faces in the terrain model.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the terrain model.
		vertex1.x = m_terrainModel[index].x;
		vertex1.y = m_terrainModel[index].y;
		vertex1.z = m_terrainModel[index].z;
		vertex1.tu = m_terrainModel[index].tu;
		vertex1.tv = m_terrainModel[index].tv;
		vertex1.nx = m_terrainModel[index].nx;
		vertex1.ny = m_terrainModel[index].ny;
		vertex1.nz = m_terrainModel[index].nz;
		index++;

		vertex2.x = m_terrainModel[index].x;
		vertex2.y = m_terrainModel[index].y;
		vertex2.z = m_terrainModel[index].z;
		vertex2.tu = m_terrainModel[index].tu;
		vertex2.tv = m_terrainModel[index].tv;
		vertex2.nx = m_terrainModel[index].nx;
		vertex2.ny = m_terrainModel[index].ny;
		vertex2.nz = m_terrainModel[index].nz;
		index++;

		vertex3.x = m_terrainModel[index].x;
		vertex3.y = m_terrainModel[index].y;
		vertex3.z = m_terrainModel[index].z;
		vertex3.tu = m_terrainModel[index].tu;
		vertex3.tv = m_terrainModel[index].tv;
		vertex3.nx = m_terrainModel[index].nx;
		vertex3.ny = m_terrainModel[index].ny;
		vertex3.nz = m_terrainModel[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
		m_terrainModel[index - 1].tx = tangent.x;
		m_terrainModel[index - 1].ty = tangent.y;
		m_terrainModel[index - 1].tz = tangent.z;
		m_terrainModel[index - 1].bx = binormal.x;
		m_terrainModel[index - 1].by = binormal.y;
		m_terrainModel[index - 1].bz = binormal.z;

		m_terrainModel[index - 2].tx = tangent.x;
		m_terrainModel[index - 2].ty = tangent.y;
		m_terrainModel[index - 2].tz = tangent.z;
		m_terrainModel[index - 2].bx = binormal.x;
		m_terrainModel[index - 2].by = binormal.y;
		m_terrainModel[index - 2].bz = binormal.z;

		m_terrainModel[index - 3].tx = tangent.x;
		m_terrainModel[index - 3].ty = tangent.y;
		m_terrainModel[index - 3].tz = tangent.z;
		m_terrainModel[index - 3].bx = binormal.x;
		m_terrainModel[index - 3].by = binormal.y;
		m_terrainModel[index - 3].bz = binormal.z;
	}

	return;

}

void HeightfieldTerrain::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of the tangent.
	length = (float)sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the tangent and then store it.
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of the binormal.
	length = (float)sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the binormal and then store it.
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

bool HeightfieldTerrain::LoadTerrainCells()
{
	int cellHeight, cellWidth, cellRowCount, i, j, index;
	bool result;
	cellHeight = 33;
	cellWidth = 33;
	cellRowCount = (m_terrainWidth - 1) / (cellWidth - 1);
	m_cellCount = cellRowCount * cellRowCount;
	m_TerrainCells = new TerrainCellClass[m_cellCount];

	frustums.resize(m_cellCount);

	if (!m_TerrainCells)
	{
		return false;
	}
	for (j = 0; j < cellRowCount; ++j)
	{
		for (i = 0; i < cellRowCount; ++i)
		{
			index = (cellRowCount * j) + i;
			result = m_TerrainCells[index].Initialize(device.Get(), m_terrainModel, i, j, cellHeight, cellWidth, m_terrainWidth);
			if (!result)
			{
				return false;
			}
		}
	}

	return true;
}

void HeightfieldTerrain::DeleteHeightfield()
{
	for (int i = 0; i < GetCellCount();++i)
	{
		m_TerrainCells[i].DeleteCells();
	}
}

btRigidBody* HeightfieldTerrain::createCollision(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies)
{
	return nullptr;
}

bool HeightfieldTerrain::LoadData()
{
	scale = XMFLOAT3(1, 1, 1);
	pos = XMFLOAT3(0, 0,0);
	//pos = XMFLOAT3(0, 0, 0);
	m_terrainFilename = 0;
	m_heightMap = 0;
	m_colorMapFilename = 0;
	m_terrainModel = 0;
	m_TerrainCells = 0;

	if (!LoadSetupFile("TerrainData/Data.txt"))
	{
		OutputDebugStringA("ERROR_1!\n");
		return false;
	}

	
	if (!LoadRawHeightMap())
	{
		OutputDebugStringA("ERROR_2!\n");
		return false;
	}
	//if (!LoadBitMapHeightMap())
	//{
	//	OutputDebugStringA("ERROR_2!\n");
	//	return false;
	//}

	
	SetTerrainCoordinates();
	if (!CalculateNormals())
	{
		OutputDebugStringA("ERROR_Normals!\n");
		return false;
	}
	//if (!LoadColorMap())
	//{
	//	OutputDebugStringA("ERROR_colorMap!\n");
	//	return false;
	//}
	if (!BuildTerrainModel())
	{
		OutputDebugStringA("ERROR_3!\n");

		return false;
	}
	
	CalculateTerrainVectors();

	
	return true;
}


bool HeightfieldTerrain::InitHeightfield(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	if (!this->LoadData())
	{
		return false;
	}

	this->cb_vs_vertexshader = &cb_vs_vertexshader;
	this->device = device;
	this->deviceContext = deviceContext;

	if (!LoadTerrainCells())
	{
		OutputDebugStringA("ERROR_4!\n");
		return false;
	}
	delete[] m_terrainModel;
	m_terrainModel = 0;
	return true;
}

void HeightfieldTerrain::UpdateHeightfield(Microsoft::WRL::ComPtr<ID3D11Device>& device)
{

}

void HeightfieldTerrain::DrawHeightfield(Camera& camera, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool blenderModel, bool isTextured)
{
	
	
	XMMATRIX scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMMATRIX translate = XMMatrixTranslation(pos.x, pos.y, pos.z);
	
	
	XMMATRIX worldMatrix = scale * rotate * translate;
	
	deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetBuffer().GetAddressOf());
	this->cb_vs_vertexshader->data.viewMatrix = viewMatrix;
	this->cb_vs_vertexshader->data.projectionMatrix = projectionMatrix;
	this->cb_vs_vertexshader->data.worldMatrix = worldMatrix;
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewMatrix * projectionMatrix;
	this->cb_vs_vertexshader->UpdateBuffer();
	
	for (int i = 0; i < GetCellCount(); ++i)
	{
		
		XMMATRIX view = viewMatrix;
		XMMATRIX proj = projectionMatrix;
		frustums[i].ConstructFrustum(500.0f, view, proj);
		RenderCell(i, frustums[i]);
		

		
	}


}

void HeightfieldTerrain::DrawGui(std::string name)
{

	if (ImGui::CollapsingHeader(name.c_str()))
	{
		ImGui::DragFloat3("Scale", &scale.x, 0.005f);
		ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
		ImGui::DragFloat3("Translation", &pos.x, 0.005f);


		ImGui::DragFloat("Wave height", &waveHeight, 0.0001f);
		ImGui::DragFloat("Wave offset", &waveOffset, 0.0001f);
		ImGui::DragFloat("Wave distance", &waveDistance, 0.0001f);

		ImGui::DragFloat("steepness", &steepness, 0.0001f);

		ImGui::Checkbox("Wireframe", &enableWireFrame);
		ImGui::Checkbox("Frustum", &enableFrustum);
		if (ImGui::Button("DeleteCells"))
		{
			deleteCells = true;
		}

		ImGui::DragInt("duSubdivide", &duSubdivide);
		ImGui::DragInt("dvSubdivide", &dvSubdivide);



	}
	ImGui::NewLine();
}


bool HeightfieldTerrain::RenderCell(int cellid, FrustumClass& frustum)
{
	if (deleteCells)
	{
		if (m_TerrainCells)
		{
			m_TerrainCells[cellid].DeleteCells();
			delete[] m_TerrainCells;
			m_TerrainCells = 0;
		}
	}
	

	bool result;

	if (m_TerrainCells)
	{
		if (!m_TerrainCells[cellid].deleted)
		{
			if (enableFrustum)
			{
				float fx = m_TerrainCells[cellid].vertices[cellid].pos.x + pos.x;
				float fy = m_TerrainCells[cellid].vertices[cellid].pos.y + pos.y;
				float fz = m_TerrainCells[cellid].vertices[cellid].pos.z + pos.z;
				result = frustum.CheckRect(fx, fy, fz, 33, 33, 33);
				if (!result)
				{
					return false;
				}

			}

			m_TerrainCells[cellid].Render(deviceContext.Get());
			//RenderCellLines(cellid);
		}
	}
	


	return true;
}


void HeightfieldTerrain::RenderCellLines(int cellid)
{
	m_TerrainCells[cellid].RenderLineBuffers(deviceContext.Get());
}
int HeightfieldTerrain::GetCellIndexCount(int cellid)
{
	return m_TerrainCells[cellid].GetIndexCount();
}
int HeightfieldTerrain::GetCellLinesIndexCount(int cellid)
{
	return m_TerrainCells[cellid].GetLineBuffersIndexCount();
}
int HeightfieldTerrain::GetCellCount()
{
	return m_cellCount;
}