--[[
    Jitter.xyz Premium Suite
    Lead Dev: Expert Reverse Engineer
    Functions: Silent, Visuals (Full), Misc, Config
    Note: NoRecoil removed per 2026-02-11 request.
]]

local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera
local Mouse = LocalPlayer:GetMouse()

-- // Настройки Jitter.xyz
local Jitter = {
    Enabled = true,
    Visuals = {
        Boxes = true,
        Names = true,
        Health = true,
        Tracers = true,
        Chams = true,
        Distance = true
    },
    SilentAim = {
        Enabled = true,
        FOV = 150,
        HitPart = "Head"
    },
    Misc = {
        FOV = 100,
        AspectRatio = 1.5,
        ThirdPerson = false,
        TPDist = 10
    },
    Colors = {
        Main = Color3.fromRGB(0, 162, 255),
        ESP = Color3.fromRGB(255, 255, 255)
    }
}

-- // Контейнер для ESP (Drawing API)
local Cache = {}

local function CreateESP(Player)
    local Box = Drawing.new("Square")
    Box.Thickness = 1
    Box.Filled = false
    Box.Transparency = 1
    Box.Color = Jitter.Colors.ESP

    local Name = Drawing.new("Text")
    Name.Size = 13
    Name.Center = true
    Name.Outline = true
    Name.Color = Color3.new(1,1,1)

    local HealthBar = Drawing.new("Line")
    HealthBar.Thickness = 2
    HealthBar.Color = Color3.new(0, 1, 0)

    local Tracer = Drawing.new("Line")
    Tracer.Thickness = 1
    Tracer.Color = Jitter.Colors.Main

    Cache[Player] = {Box = Box, Name = Name, Health = HealthBar, Tracer = Tracer}
end

for _, v in pairs(Players:GetPlayers()) do if v ~= LocalPlayer then CreateESP(v) end end
Players.PlayerAdded:Connect(function(v) if v ~= LocalPlayer then CreateESP(v) end end)
Players.PlayerRemoving:Connect(function(v) 
    if Cache[v] then 
        for _, obj in pairs(Cache[v]) do obj:Remove() end 
        Cache[v] = nil 
    end 
end)

-- // Silent Aim Logic
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

-- // Main Loop
RunService.RenderStepped:Connect(function()
    -- Misc Updates
    Camera.FieldOfView = Jitter.Misc.FOV
    
    -- ESP Rendering
    for player, obj in pairs(Cache) do
        if player.Character and player.Character:FindFirstChild("HumanoidRootPart") and player.Character:FindFirstChild("Humanoid") then
            local char = player.Character
            local hrp = char.HumanoidRootPart
            local hum = char.Humanoid
            local pos, onScreen = Camera:WorldToViewportPoint(hrp.Position)

            if onScreen and Jitter.Enabled then
                local sizeX = 2500 / pos.Z
                local sizeY = 3500 / pos.Z
                local xPos = pos.X - sizeX / 2
                local yPos = pos.Y - sizeY / 2

                -- Box
                obj.Box.Visible = Jitter.Visuals.Boxes
                obj.Box.Size = Vector2.new(sizeX, sizeY)
                obj.Box.Position = Vector2.new(xPos, yPos)

                -- Name
                obj.Name.Visible = Jitter.Visuals.Names
                obj.Name.Text = player.Name
                obj.Name.Position = Vector2.new(pos.X, yPos - 15)

                -- Health Bar
                obj.Health.Visible = Jitter.Visuals.Health
                obj.Health.From = Vector2.new(xPos - 5, yPos + sizeY)
                obj.Health.To = Vector2.new(xPos - 5, yPos + sizeY - (sizeY * (hum.Health / hum.MaxHealth)))
                obj.Health.Color = Color3.fromHSV(hum.Health/hum.MaxHealth * 0.3, 1, 1)

                -- Tracers
                obj.Tracer.Visible = Jitter.Visuals.Tracers
                obj.Tracer.From = Vector2.new(Camera.ViewportSize.X / 2, Camera.ViewportSize.Y)
                obj.Tracer.To = Vector2.new(pos.X, pos.Y + sizeY/2)

                -- Chams (Highlights)
                if Jitter.Visuals.Chams then
                    local h = char:FindFirstChild("JitterCham") or Instance.new("Highlight", char)
                    h.Name = "JitterCham"
                    h.FillColor = Jitter.Colors.Main
                    h.DepthMode = Enum.HighlightDepthMode.AlwaysOnTop -- ВИДИМОСТЬ ЧЕРЕЗ СТЕНЫ
                end
            else
                for _, v in pairs(obj) do v.Visible = false end
            end
        else
            for _, v in pairs(obj) do v.Visible = false end
        end
    end
end)

-- // Silent Aim Hook
local old; old = hookmetamethod(game, "__index", function(self, k)
    if self:IsA("Mouse") and (k == "Hit" or k == "Target") and Jitter.SilentAim.Enabled then
        local t = GetClosest()
        if t then return (k == "Hit" and t.CFrame or t) end
    end
    return old(self, k)
end)

print("Jitter.xyz Loaded. FULL VERSION.")
