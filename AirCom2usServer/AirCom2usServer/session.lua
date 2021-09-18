myid = -1;
local ENEMY_TYPE = {ENEMY_Plane1=0, ENEMY_Plane2=1, ENEMY_Plane3=2, ENEMY_Boss1=3, ENEMY_Boss2=4 };

enemys0 = {
  ["0"] = {type = ENEMY_TYPE.ENEMY_Plane1, time = 1000, x = 0, y = 5},
  ["1"] = {type = ENEMY_TYPE.ENEMY_Plane2, time = 2000, x = 0, y = 5 },
  ["2"] = {type = ENEMY_TYPE.ENEMY_Plane3, time = 3000, x = 0, y = 5 },
  ["3"] = {type = ENEMY_TYPE.ENEMY_Plane1, time = 4000, x = 0, y = 5 },
  ["4"] = {type = ENEMY_TYPE.ENEMY_Plane2, time = 5000, x = 0, y = 5 },
  ["5"] = {type = ENEMY_TYPE.ENEMY_Plane3, time = 6000, x = 0, y = 5 },
  ["6"] = {type = ENEMY_TYPE.ENEMY_Plane1, time = 7000, x = 0, y = 5 },
  ["7"] = {type = ENEMY_TYPE.ENEMY_Plane2, time = 8000, x = 0, y = 5 },
  ["8"] = {type = ENEMY_TYPE.ENEMY_Plane3, time = 9000, x = 0, y = 5 },
  ["9"] = {type = ENEMY_TYPE.ENEMY_Plane1, time = 10000, x = 0, y = 5 },
  ["10"] = {type = ENEMY_TYPE.ENEMY_Plane2, time = 11000, x = 0, y = 5 }
}

enemys1 = {
  ["0"] = {type = ENEMY_TYPE.ENEMY_Plane1, time = 1000, x = 0, y = 5},
  ["1"] = {type = ENEMY_TYPE.ENEMY_Plane2, time = 1500, x = 1, y = 5 },
  ["2"] = {type = ENEMY_TYPE.ENEMY_Plane3, time = 2000, x = 0, y = 5 },
  ["3"] = {type = ENEMY_TYPE.ENEMY_Plane1, time = 2500, x = -1, y = 5 },
  ["4"] = {type = ENEMY_TYPE.ENEMY_Plane2, time = 3000, x = -2, y = 5 },
  ["5"] = {type = ENEMY_TYPE.ENEMY_Plane3, time = 4000, x = 0, y = 5 },
  ["6"] = {type = ENEMY_TYPE.ENEMY_Plane1, time = 4300, x = 0, y = 5 },
  ["7"] = {type = ENEMY_TYPE.ENEMY_Plane2, time = 4600, x = -2, y = 5 },
  ["8"] = {type = ENEMY_TYPE.ENEMY_Plane3, time = 4900, x = 0, y = 5 },
  ["9"] = {type = ENEMY_TYPE.ENEMY_Plane1, time = 6000, x = 2, y = 5 },
  ["10"] = {type = ENEMY_TYPE.ENEMY_Plane2, time = 7000, x = 0, y = 5 }
}
function set_uid(x)
	myid = x;
end

function tablelength(T)
  local count = 0
  for _ in pairs(T) do count = count + 1 end
  return count
end
