#define Max_Compute_Shader_Threads		512

cbuffer constantVariables : register(b0){
	uint groupDimX;
	uint groupDimY;
	uint maxParticles;
	uint pad;
}

cbuffer dynamicVariables : register(b1)
{	
	float3	attractor;	
	float	frameTime;
	
//	bool pulling;
//	bool limitingPosition;
//	bool gravity;
//	bool resistence;	

	float4 data;
	
	float forceFactor;
	float resistenceForce;
	int areaLimit;	
	int pad1;
	
	
}

struct InstanceType
{
	// The particle position
	float4 position;

	// The particle last position
	float4 lastPosition;

	// The particle acceleration
	float4 acceleration;	
};

RWStructuredBuffer<InstanceType> srcParticleBuffer : register(u0);

[numthreads(Max_Compute_Shader_Threads, 1, 1)]
void computeShader( uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex  )
{
		//Calculate the ID of this Thread
		uint ID = groupID.x * Max_Compute_Shader_Threads + groupID.y * groupDimX * Max_Compute_Shader_Threads + groupID.z * groupDimX * groupDimY * Max_Compute_Shader_Threads + groupIndex; 
		
		//Every thread renders a particle. 
		//If there are more threads than particles then stop here.
		if(ID < maxParticles)
		{
			InstanceType p = srcParticleBuffer[ID];
			float3 acceleration;
			float3 tempCurPos;
			float3 tempOldPos;
			float3 tempAcc;
			float3 force;
			float multiplyValue;
			
			// Save the positions
			tempCurPos = p.position;
			tempOldPos = p.lastPosition;	
			tempAcc = p.acceleration;
			
			// Set the force
			if(!data.x)
			{
				force = attractor - tempCurPos * 1.0f;
			}
			else
			{
				// Check the gravity
				if(data.z)
				{
					force = float3(0, -10.05, 0);
				}
				else
				{
					force = float3(0, 0, 0);
				}
			}
			
			// Increment the force
			tempAcc += force;
			
			// Check if we need to normalize the acceleration
			if(length(tempAcc))
			{
				acceleration = normalize(tempAcc);
			}
			else
			{
				acceleration = tempAcc;
			}
			
			// Set the multiply value
			if(data.w)
			{
				multiplyValue = resistenceForce;
			}
			else
			{
				multiplyValue = 1.0f;
			}
			
			// Set the new position
			tempOldPos = tempCurPos;
			tempCurPos = 2.0*tempCurPos - tempOldPos + acceleration *frameTime * 200 * forceFactor * multiplyValue;
			
			// Check if we are limiting the position or the acceleration
			if(data.y)
			{
				//Keep all the particles inside
				if(length(tempCurPos) > areaLimit){
					float3 norm = normalize(tempCurPos);
					tempCurPos = float4(norm*areaLimit, 1);
				}		
			}
			else
			{
				//Keep all the particles inside
				if(length(tempAcc) > areaLimit){
					float3 norm = normalize(tempAcc);
					tempAcc = float4(norm*areaLimit, 1);
				}
			}
					
			// Set the values inside the particle
			p.position = float4(tempCurPos, 1);
			p.lastPosition = float4(tempOldPos, 1);
			p.acceleration = float4(tempAcc, 1);

			srcParticleBuffer[ID] = p;
		}
}