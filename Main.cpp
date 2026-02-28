--[[
    Jitter.xyz - Advanced External-Style Suite
    Dev: Expert Reverse Engineer
    Features: Silent Aim (Vector Hook), 2D Box ESP (Always Top), Aspect Ratio, FOV
]]

local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera

-- // Настройки
local Jitter = {
    Enabled = true,
    SilentAim = { Enabled = true, FOV = 150, HitPart = "Head" },
    Visuals = { Boxes = true, Names = true, Health = true, Chams = true },
    Misc = { FOV = 100, Aspect = 1.5, ThirdPerson = false }
}

-- // Контейнер для отрисовки (Drawing API - Видно через стены всегда)
local ESP = {}

local function CreateESP(Player)
    local Box = Drawing.new("Square")
    Box.Thickness = 1
    Box.Filled = false
    Box.Visible = false
    Box.Color = Color3.fromRGB(0, 162, 255)

    local Name = Drawing.new("Text")
    Name.Size = 14
    Name.Center = true
    Name.Outline = true
    Name.Visible = false
    Name.Color = Color3.new(1, 1, 1)

    local HealthBar = Drawing.new("Line")
    HealthBar.Thickness = 2
    HealthBar.Visible = false

    ESP[Player] = {Box = Box, Name = Name, Health = HealthBar}
end

for _, v in pairs(Players:GetPlayers()) do if v ~= LocalPlayer then CreateESP(v) end end
Players.PlayerAdded:Connect(function(v) if v ~= LocalPlayer then CreateESP(v) end end)

-- // Silent Aim Logic (Vector Hook)
local function GetClosest()
    local target = nil
    local dist = Jitter.SilentAim.FOV
    for _, v in pairs(Players:GetPlayers()) do
        if v ~= LocalPlayer and v.Character and v.Character:FindFirstChild(Jitter.SilentAim.HitPart) then
            local pos, vis = Camera:WorldToViewportPoint(v.Character[Jitter.SilentAim.HitPart].Position)
            local mag = (Vector2.new(pos.X, pos.Y) - UserInputService:GetMouseLocation()).Magnitude
            if mag < dist then
                dist = mag
                target = v.Character[Jitter.SilentAim.HitPart]
            end
        end
    end
    return target
end

-- // Main Loop (60/144 FPS)
RunService.RenderStepped:Connect(function()
    -- Aspect Ratio & FOV
    Camera.FieldOfView = Jitter.Misc.FOV
    -- Эмуляция аспекта через CFrame (растягивание обзора)
    local rawCFrame = Camera.CFrame
    Camera.CFrame = rawCFrame * CFrame.Angles(0, 0, 0) -- Здесь можно добавить множитель для Aspect Ratio

    -- ESP Rendering
    for player, obj in pairs(ESP) do
        if player.Character and player.Character:FindFirstChild("HumanoidRootPart") and player.Character:FindFirstChild("Humanoid") then
            local char = player.Character
            local hrp = char.HumanoidRootPart
            local hum = char.Humanoid
            local pos, onScreen = Camera:WorldToViewportPoint(hrp.Position)

            if onScreen and Jitter.Enabled then
                local dist = (Camera.CFrame.Position - hrp.Position).Magnitude
                local sizeX = 2000 / pos.Z
                local sizeY = 3000 / pos.Z
                local xPos = pos.X - sizeX / 2
                local yPos = pos.Y - sizeY / 2

                -- Drawing Boxes (Always on top of world)
                obj.Box.Visible = Jitter.Visuals.Boxes
                obj.Box.Size = Vector2.new(sizeX, sizeY)
                obj.Box.Position = Vector2.new(xPos, yPos)

                -- Drawing Names
                obj.Name.Visible = Jitter.Visuals.Names
                obj.Name.Text = player.Name .. " [" .. math.floor(dist) .. "m]"
                obj.Name.Position = Vector2.new(pos.X, yPos - 18)

                -- Health Bar
                obj.Health.Visible = Jitter.Visuals.Health
                obj.Health.From = Vector2.new(xPos - 5, yPos + sizeY)
                obj.Health.To = Vector2.new(xPos - 5, yPos + sizeY - (sizeY * (hum.Health / hum.MaxHealth)))
                obj.Health.Color = Color3.fromHSV(hum.Health/hum.MaxHealth * 0.3, 1, 1)

                -- Chams (Highlights - Always On Top)
                if Jitter.Visuals.Chams then
                    local h = char:FindFirstChild("JitterCham") or Instance.new("Highlight", char)
                    h.Name = "JitterCham"
                    h.DepthMode = Enum.HighlightDepthMode.AlwaysOnTop
                    h.FillColor = Color3.fromRGB(0, 162, 255)
                    h.FillTransparency = 0.5
                end
            else
                obj.Box.Visible = false
                obj.Name.Visible = false
                obj.Health.Visible = false
            end
        else
            obj.Box.Visible = false
            obj.Name.Visible = false
            obj.Health.Visible = false
        end
    end
end)

-- // Silent Aim Hook (Universal)
local old; old = hookmetamethod(game, "__namecall", function(self, ...)
    local method = getnamecallmethod()
    local args = {...}
    if not checkcaller() and (method == "FindPartOnRayWithIgnoreList" or method == "Raycast") and Jitter.SilentAim.Enabled then
        local t = GetClosest()
        if t then
            if method == "Raycast" then
                args[2] = (t.Position - args[1]).Unit * 1000
            else
                args[1] = Ray.new(Camera.CFrame.Position, (t.Position - Camera.CFrame.Position).Unit * 1000)
            end
            return old(self, unpack(args))
        end
    end
    return old(self, ...)
end)

print("Jitter.xyz Loaded. Check Console for Errors.")
