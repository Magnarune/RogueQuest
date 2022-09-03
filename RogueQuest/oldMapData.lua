return {
  version = "1.9",
  luaversion = "5.1",
  tiledversion = "1.9.1",
  class = "",
  orientation = "orthogonal",
  renderorder = "right-down",
  width = 7,
  height = 7,
  tilewidth = 32,
  tileheight = 32,
  nextlayerid = 3,
  nextobjectid = 1,
  properties = {},
  tilesets = {
    {
      name = "Ground",
      firstgid = 1,
      filename = "C:/Users/BloodworthJr/source/repos/RogueQuest/RogueQuest/Maps/tallgrass.png"
    },
    {
      name = "fishing",
      firstgid = 19,
      filename = "C:/Users/BloodworthJr/source/repos/RogueQuest/RogueQuest/Maps/farming_fishing.png"
    }
  },
  layers = {
    {
      type = "tilelayer",
      x = 0,
      y = 0,
      width = 7,
      height = 7,
      id = 1,
      name = "Tile Layer 1",
      class = "",
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      parallaxx = 1,
      parallaxy = 1,
      properties = {},
      encoding = "lua",
      data = {
        11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,
        11,11,11,11,11,11,11,
        11,11,11,11,11,11,19
      }
    },
    {
      type = "tilelayer",
      x = 0,
      y = 0,
      width = 7,
      height = 7,
      id = 2,
      name = "Tile Layer 2",
      class = "",
      visible = true,
      opacity = 1,
      offsetx = 0,
      offsety = 0,
      parallaxx = 1,
      parallaxy = 1,
      properties = {},
      encoding = "lua",
      data = {
        0,0,0,0,0,0,0,
        0,70,70,70,70,0,0,
        0,70,0,0,0,0,0,
        0,0,70,224,0,0,0,
        0,0,0,324,0,0,0,
        0,0,24,0,0,0,0,
        0,0,24,0,0,0,0
      }
    }
  }
}
