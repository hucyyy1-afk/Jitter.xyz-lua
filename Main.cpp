--[[
    Jitter.xyz - ELITE BUILD v3.0
    Functions: Silent Aim, Box/Name/Health ESP, Chams, FOV & Extended Aspect Ratio
    Status: NoRecoil REMOVED.
]]

local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera
local Mouse = LocalPlayer:GetMouse()

-- // Глобальный конфиг
_G.Jitter = {
    SilentAim = { Enabled = true, FOV = 150, HitPart = "Head" },
    Visuals = { Boxes = true, Names = true, Health = true, Chams = true },
    Misc = { FOV = 100, Aspect = 1.0 } -- Дефолт
}

-- // UI Framework
local Screen = Instance.new("ScreenGui", (gethui and gethui()) or game:GetService("CoreGui"))
Screen.Name = "Jitter_Elite"

local Main = Instance.new("Frame", Screen)
Main.Size = UDim2.new(0, 550, 0, 480)
Main.Position = UDim2.new(0.5, -275, 0.5, -240)
Main.BackgroundColor3 = Color3.fromRGB(12, 14, 18)
Main.BorderSizePixel = 0
Main.Active = true
Main.Draggable = true

local Container = Instance.new("ScrollingFrame", Main)
Container.Size = UDim2.new(1, -180, 1, -20)
Container.Position = UDim2.new(0, 170, 0, 10)
Container.BackgroundTransparency = 1
Container.CanvasSize = UDim2.new(0, 0, 2, 0)
Container.ScrollBarThickness = 2

local UIList = Instance.new("UIListLayout", Container)
UIList.Padding = UDim.new(0, 12)

-- // UI Helpers
local function AddToggle(text, folder, setting)
    local Btn = Instance.new("TextButton", Container)
    Btn.Size = UDim2.new(1, -15, 0, 35)
    Btn.BackgroundColor3 = _G.Jitter[folder][setting] and Color3.fromRGB(0, 162, 255) or Color3.fromRGB(28, 30, 35)
    Btn.Text = "  " .. text; Btn.TextColor3 = Color3.new(1,1,1); Btn.Font = Enum.Font.GothamSemibold; Btn.TextXAlignment = Enum.TextXAlignment.Left
    local Corner = Instance.new("UICorner", Btn); Corner.CornerRadius = UDim.new(0, 4)
    
    Btn.MouseButton1Click:Connect(function()
        _G.Jitter[folder][setting] = not _G.Jitter[folder][setting]
        Btn.BackgroundColor3 = _G.Jitter[folder][setting] and Color3.fromRGB(0, 162, 255) or Color3.fromRGB(28, 30, 35)
    end)
end

local function AddSlider(text, min, max, folder, setting)
    local Frame = Instance.new("Frame", Container); Frame.Size = UDim2.new(1, -15, 0, 45); Frame.BackgroundTransparency = 1
    local Lbl = Instance.new("TextLabel", Frame); Lbl.Text = text..": ".._G.Jitter[folder][setting]; Lbl.Size = UDim2.new(1,0,0,20); Lbl.TextColor3 = Color3.new(1,1,1); Lbl.BackgroundTransparency = 1; Lbl.Font = Enum.Font.Gotham; Lbl.TextXAlignment = Enum.TextXAlignment.Left
    local BG = Instance.new("Frame", Frame); BG.Size = UDim2.new(1,0,0,6); BG.Position = UDim2.new(0,0,0,25); BG.BackgroundColor3 = Color3.fromRGB(35,35,40)
    local Fill = Instance.new("Frame", BG); Fill.Size = UDim2.new((_G.Jitter[folder][setting]-min)/(max-min),0,1,0); Fill.BackgroundColor3 = Color3.fromRGB(0, 162, 255)
    
    BG.InputBegan:Connect(function(input)
        if input.UserInputType == Enum.UserInputType.MouseButton1 then
            local function Update()
                local pos = math.clamp((UserInputService:GetMouseLocation().X - BG.AbsolutePosition.X) / BG.AbsoluteSize.X, 0, 1)
                local val = math.floor((min + (max - min) * pos) * 10) / 10
                _G.Jitter[folder][setting] = val
                Fill.Size = UDim2.new(pos, 0, 1, 0)
                Lbl.Text = text..": "..val
            end
            local move; move = RunService.RenderStepped:Connect(Update)
            local release; release = UserInputService.InputEnded:Connect(function(i)
                if i.UserInputType == Enum.UserInputType.MouseButton1 then move:Disconnect(); release:Disconnect() end
            end)
        end
    end)
end

-- // Наполнение Меню
AddToggle("Silent Aim", "SilentAim", "Enabled")
AddToggle("Box ESP (Wallhack)", "Visuals", "Boxes")
AddToggle("Player Names", "Visuals", "Names")
AddToggle("Health Bar", "Visuals", "Health")
AddToggle("Chams (Fill)", "Visuals", "Chams")
AddSlider("Aspect Ratio (Stretch)", 0.1, 5.0, "Misc", "Aspect") -- Максимум 5.0 теперь
AddSlider("Field of View", 60, 120, "Misc", "FOV")

-- // ESP Logic (Drawing API)
local ESP_Table = {}
local function CreateESP(p)
    local Box = Drawing.new("Square"); Box.Thickness = 1; Box.Color = Color3.fromRGB(0, 162, 255)
    local Name = Drawing.new("Text"); Name.Size = 13; Name.Center = true; Name.Outline = true; Name.Color = Color3.new(1,1,1)
    local Health = Drawing.new("Line"); Health.Thickness = 2
    ESP_Table[p] = {Box = Box, Name = Name, Health = Health}
end
for _, v in pairs(Players:GetPlayers()) do if v ~= LocalPlayer then CreateESP(v) end end
Players.PlayerAdded:Connect(function(v) if v ~= LocalPlayer then CreateESP(v) end end)

-- // Runtime
RunService.RenderStepped:Connect(function()
    -- Aspect Ratio Logic
    Camera.FieldOfView = _G.Jitter.Misc.FOV
    local a = _G.Jitter.Misc.Aspect
    Camera.CFrame = Camera.CFrame * CFrame.new(0,0,0, a, 0, 0, 0, 1, 0, 0, 0, 1)

    -- ESP Rendering
    for p, obj in pairs(ESP_Table) do
        if p.Character and p.Character:FindFirstChild("HumanoidRootPart") and p.Character:FindFirstChild("Humanoid") then
            local char = p.Character; local hrp = char.HumanoidRootPart; local hum = char.Humanoid
            local pos, on = Camera:WorldToViewportPoint(hrp.Position)
            if on and _G.Jitter.Visuals.Boxes then
                local sX, sY = 2000/pos.Z, 3000/pos.Z
                local x, y = pos.X - sX/2, pos.Y - sY/2
                -- Box & Name & Health
                obj.Box.Visible = _G.Jitter.Visuals.Boxes; obj.Box.Size = Vector2.new(sX, sY); obj.Box.Position = Vector2.new(x, y)
                obj.Name.Visible = _G.Jitter.Visuals.Names; obj.Name.Text = p.Name; obj.Name.Position = Vector2.new(pos.X, y - 15)
                obj.Health.Visible = _G.Jitter.Visuals.Health; obj.Health.From = Vector2.new(x-5, y+sY); obj.Health.To = Vector2.new(x-5, y+sY-(sY*(hum.Health/hum.MaxHealth))); obj.Health.Color = Color3.fromHSV(hum.Health/hum.MaxHealth*0.3, 1, 1)
                -- Chams
                local c = char:FindFirstChild("JitterCham") or Instance.new("Highlight", char)
                c.Name = "JitterCham"; c.Enabled = _G.Jitter.Visuals.Chams; c.DepthMode = Enum.HighlightDepthMode.AlwaysOnTop; c.FillColor = Color3.fromRGB(0, 162, 255)
            else for _, v in pairs(obj) do v.Visible = false end end
        else for _, v in pairs(obj) do v.Visible = false end end
    end
end)

-- // Silent Aim Target
local function GetClosest()
    local t, d = nil, _G.Jitter.SilentAim.FOV
    for _, v in pairs(Players:GetPlayers()) do
        if v ~= LocalPlayer and v.Character and v.Character:FindFirstChild(_G.Jitter.SilentAim.HitPart) then
            local pos, vis = Camera:WorldToViewportPoint(v.Character[_G.Jitter.SilentAim.HitPart].Position)
            local mag = (Vector2.new(pos.X, pos.Y) - UserInputService:GetMouseLocation()).Magnitude
            if vis and mag < d then d = mag; t = v.Character[_G.Jitter.SilentAim.HitPart] end
        end
    end
    return t
end

-- // Silent Aim Hook (__namecall)
local old; old = hookmetamethod(game, "__namecall", function(self, ...)
    local m = getnamecallmethod()
    local args = {...}
    if (m == "Raycast" or m == "FindPartOnRayWithIgnoreList") and _G.Jitter.SilentAim.Enabled and not checkcaller() then
        local t = GetClosest()
        if t then
            if m == "Raycast" then args[2] = (t.Position - args[1]).Unit * 1000
            else args[1] = Ray.new(Camera.CFrame.Position, (t.Position - Camera.CFrame.Position).Unit * 1000) end
            return old(self, unpack(args))
        end
    end
    return old(self, ...)
end)

UserInputService.InputBegan:Connect(function(i) if i.KeyCode == Enum.KeyCode.Insert then Screen.Enabled = not Screen.Enabled; UserInputService.MouseIconEnabled = Screen.Enabled end end)
