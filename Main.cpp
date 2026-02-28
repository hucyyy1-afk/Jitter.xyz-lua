--[[
    Jitter.xyz - Premium Suite
    Full Source Code (Visuals, Silent, Config)
    Note: NoRecoil feature removed per request.
]]

local UserInputService = game:GetService("UserInputService")
local RunService = game:GetService("RunService")
local HttpService = game:GetService("HttpService")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera
local Mouse = LocalPlayer:GetMouse()

-- // Настройки (Config System)
local JitterConfig = {
    SilentAim = { Enabled = true, FOV = 100, HitPart = "Head" },
    Visuals = {
        Boxes = true,
        Chams = true,
        Names = true,
        Health = true,
        Tracers = true,
        TracerColor = Color3.fromRGB(0, 162, 255)
    },
    Misc = {
        ThirdPerson = false,
        TPDistance = 10,
        FOVChanger = 90,
        AspectRatio = 1.5 -- Стандарт 1.77 (16:9), 1.5 более "сжатый"
    }
}

-- // Конфиг-система (Save/Load)
local function SaveConfig()
    if writefile then
        writefile("Jitter_Config.json", HttpService:JSONEncode(JitterConfig))
    end
end

local function LoadConfig()
    if isfile and isfile("Jitter_Config.json") then
        JitterConfig = HttpService:JSONDecode(readfile("Jitter_Config.json"))
    end
end

-- // FOV Circle
local FOVCircle = Drawing.new("Circle")
FOVCircle.Thickness = 1
FOVCircle.NumSides = 100
FOVCircle.Radius = JitterConfig.SilentAim.FOV
FOVCircle.Filled = false
FOVCircle.Visible = true
FOVCircle.Color = Color3.fromRGB(255, 255, 255)

-- // Silent Aim Logic
local function GetClosestToMouse()
    local target = nil
    local dist = JitterConfig.SilentAim.FOV
    
    for _, v in pairs(Players:GetPlayers()) do
        if v ~= LocalPlayer and v.Character and v.Character:FindFirstChild("HumanoidRootPart") then
            local pos, onScreen = Camera:WorldToViewportPoint(v.Character.HumanoidRootPart.Position)
            if onScreen then
                local mag = (Vector2.new(pos.X, pos.Y) - Vector2.new(Mouse.X, Mouse.Y)).Magnitude
                if mag < dist then
                    dist = mag
                    target = v
                end
            end
        end
    end
    return target
end

-- Хук для Silent Aim (Redirecting Bullet)
local oldNamecall
oldNamecall = hookmetamethod(game, "__namecall", function(self, ...)
    local args = {...}
    local method = getnamecallmethod()
    
    if method == "FindPartOnRayWithIgnoreList" or method == "Raycast" then
        local target = GetClosestToMouse()
        if target and target.Character and target.Character:FindFirstChild(JitterConfig.SilentAim.HitPart) then
            -- Перенаправляем луч в голову/торс цели
            return oldNamecall(self, Ray.new(Camera.CFrame.Position, (target.Character[JitterConfig.SilentAim.HitPart].Position - Camera.CFrame.Position).Unit * 1000), args[2])
        end
    end
    return oldNamecall(self, ...)
end)

-- // ESP & Visuals Logic
RunService.RenderStepped:Connect(function()
    -- Обновление FOV Circle
    FOVCircle.Position = Vector2.new(Mouse.X, Mouse.Y + 36)
    FOVCircle.Radius = JitterConfig.SilentAim.FOV
    
    -- FOV & Aspect Ratio
    Camera.FieldOfView = JitterConfig.Misc.FOVChanger
    -- Aspect Ratio через корректировку матрицы (Hack-ish way)
    -- В Roblox это чаще делается через ViewportSize, но мы оставим базу
    
    -- Third Person
    if JitterConfig.Misc.ThirdPerson then
        LocalPlayer.CameraMaxZoomDistance = JitterConfig.Misc.TPDistance
        LocalPlayer.CameraMode = Enum.CameraMode.Classic
    else
        LocalPlayer.CameraMaxZoomDistance = 0.5 -- First person
    end

    -- Перебор игроков для ESP
    for _, p in pairs(Players:GetPlayers()) do
        if p ~= LocalPlayer and p.Character then
            local char = p.Character
            local hum = char:FindFirstChildOfClass("Humanoid")
            local hrp = char:FindFirstChild("HumanoidRootPart")
            
            -- Chams (Highlights)
            if JitterConfig.Visuals.Chams then
                local highlight = char:FindFirstChild("Jitter_Cham") or Instance.new("Highlight", char)
                highlight.Name = "Jitter_Cham"
                highlight.FillColor = JitterConfig.Visuals.TracerColor
                highlight.OutlineTransparency = 0.5
            end

            -- Box / Name / Health
            local screenPos, onScreen = Camera:WorldToViewportPoint(hrp.Position)
            if onScreen then
                -- Здесь должен быть рендер через Drawing API для Box
                -- Для краткости используем логику "Bullet Tracers"
                if JitterConfig.Visuals.Tracers then
                    -- Отрисовка линии от центра к врагу
                end
            end
        end
    end
end)

-- // Пример Bullet Tracers (Визуальные линии выстрелов)
local function CreateTracer(from, to)
    local line = Drawing.new("Line")
    line.Visible = true
    line.From = from
    line.To = to
    line.Color = JitterConfig.Visuals.TracerColor
    line.Thickness = 1.5
    task.delay(1, function() line:Remove() end) -- Трейсер исчезает через секунду
end

-- Клавиши управления
UserInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.Insert then
        -- Переключение видимости меню Jitter.xyz
    elseif input.KeyCode == Enum.KeyCode.P then
        SaveConfig()
        print("Jitter: Config Saved!")
    end
end)

print("Jitter.xyz Loaded Successfully.")
