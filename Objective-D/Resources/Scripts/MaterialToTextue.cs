using UnityEngine;
using System.Collections.Generic;
using System.IO;

public class TextureAtlasGenerator : MonoBehaviour
{
    public Material[] materials;  // 변환할 여러 개의 Material
    public int textureSize = 512; // 개별 텍스처 크기

    void Start()
    {
        GenerateTextureAtlas();
    }

    void GenerateTextureAtlas()
    {
        if (materials.Length == 0)
        {
            Debug.LogError("Material이 설정되지 않았습니다!");
            return;
        }

        List<Texture2D> textures = new List<Texture2D>();

        // Material에서 Albedo Texture 가져오기
        foreach (Material mat in materials)
        {
            Texture2D texture = mat.mainTexture as Texture2D;

            if (texture == null)
            {
                // 만약 Material에 텍스처가 없으면 단색 Texture 생성
                Texture2D newTexture = new Texture2D(textureSize, textureSize);
                Color matColor = mat.color;
                Color[] pixels = new Color[textureSize * textureSize];

                for (int i = 0; i < pixels.Length; i++)
                {
                    pixels[i] = matColor;
                }

                newTexture.SetPixels(pixels);
                newTexture.Apply();
                texture = newTexture;
            }

            textures.Add(texture);
        }

        // 텍스처들을 하나의 아틀라스로 병합 (UV 정보 반환)
        Rect[] uvRects = new Rect[textures.Count];
        Texture2D atlasTexture = new Texture2D(2048, 2048);
        uvRects = atlasTexture.PackTextures(textures.ToArray(), 2, 2048);

        // 저장
        byte[] bytes = atlasTexture.EncodeToPNG();
        string path = Application.dataPath + "/TextureAtlas.png";
        File.WriteAllBytes(path, bytes);

        Debug.Log("Texture Atlas saved to: " + path);

        // UV 정보 출력
        for (int i = 0; i < uvRects.Length; i++)
        {
            Debug.Log($"Material {i} UV Rect: {uvRects[i]}");
        }
    }
}
