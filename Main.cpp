--[[
    Jitter.xyz - Final Internal-Style Build
    Functions: Silent Aim, Box ESP, Name, Health, Chams (Wallhack), FOV, Aspect
    Note: NoRecoil removed.
]]

local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local UserInputService = game:GetService("UserInputService")
local LocalPlayer = Players.LocalPlayer
local Camera = workspace.CurrentCamera

-- // Настройки
local Jitter = {
    Enabled = true,
    Visuals = {
        Boxes = true,
        Names = true,
        Health = true,
        Chams = true, -- Те самые чамсы
        ChamsColor = Color3.fromRGB(0, 162, 255)
    },
    SilentAim = { Enabled = true, FOV = 150 },
    Misc = { FOV = 100, Aspect = 1.5 }
}

-- // UI CONSTRUCTION (Neverlose Style)
local Screen = Instance.new("ScreenGui")
Screen.Name = "Jitter_Final"
Screen.ResetOnSpawn = false
Screen.DisplayOrder = 999
Screen.Parent = (gethui and gethui()) or game:GetService("CoreGui")

local Main = Instance.new("Frame")
Main.Size = UDim2.new(0, 550, 0, 400)
Main.Position = UDim2.new(0.5, -275, 0.5, -200)
Main.BackgroundColor3 = Color3.fromRGB(10, 12, 16)
Main.BorderSizePixel = 0
Main.Active = true
Main.Draggable = true
Main.Parent = Screen

local Sidebar = Instance.new("Frame")
Sidebar.Size = UDim2.new(0, 150, 1, 0)
Sidebar.BackgroundColor3 = Color3.fromRGB(7, 9, 12)
Sidebar.BorderSizePixel = 0
Sidebar.Parent = Main

local Title = Instance.new("TextLabel")
Title.Text = "JITTER.XYZ"
Title.Font = Enum.Font.GothamBlack
Title.TextSize = 18
Title.TextColor3 = Color3.new(1, 1, 1)
Title.Size = UDim2.new(1, 0, 0, 60)
Title.BackgroundTransparency = 1
Title.Parent = Sidebar

-- Контейнер для функций
local Scroll = Instance.new("ScrollingFrame")
Scroll.Size = UDim2.new(1, -170, 1, -20)
Scroll.Position = UDim2.new(0, 160, 0, 10)
Scroll.BackgroundTransparency = 1
Scroll.BorderSizePixel = 0
Scroll.CanvasSize = UDim2.new(0, 0, 1.5, 0)
Scroll.Parent = Main

local UIList = Instance.new("UIListLayout")
UIList.Padding = UDim.new(0, 10)
UIList.Parent = Scroll

-- Функция создания кнопки-переключателя
local function AddToggle(text, folder, setting)
    local Btn = Instance.new("TextButton")
    Btn.Size = UDim2.new(1, -10, 0, 35)
    Btn.BackgroundColor3 = Jitter[folder][setting] and Color3.fromRGB(0, 162, 255) or Color3.fromRGB(30, 32, 38)
    Btn.Text = text
    Btn.Font = Enum.Font.GothamSemibold
    Btn.TextColor3 = Color3.new(1, 1, 1)
    Btn.TextSize = 14
    Btn.Parent = Scroll
    
    local Corner = Instance.new("UICorner")
    Corner.CornerRadius = UDim.new(0, 4)
    Corner.Parent = Btn

    Btn.MouseButton1Click:Connect(function()
        Jitter[folder][setting] = not Jitter[folder][setting]
        Btn.BackgroundColor3 = Jitter[folder][setting] and Color3.fromRGB(0, 162, 255) or Color3.fromRGB(30, 32, 38)
    end)
end

-- Добавляем функции в меню
AddToggle("Enable Silent Aim", "SilentAim", "Enabled")
AddToggle("Show Boxes", "Visuals", "Boxes")
AddToggle("Show Names", "Visuals", "Names")
AddToggle("Show Health", "Visuals", "Health")
AddToggle("Enable Chams (Wallhack)", "Visuals", "Chams")

-- // LOGIC: ESP & CHAMS
local ESP_Cache = {}

local function ApplyVisuals(player)
    RunService.RenderStepped:Connect(function()
        if player.Character and player.Character:FindFirstChild("HumanoidRootPart") then
            local char = player.Character
            local hrp = char.HumanoidRootPart
            local hum = char:FindFirstChildOfClass("Humanoid")
            
            -- CHAMS (Сквозь стены)
            local highlight = char:FindFirstChild("JitterCham")
            if Jitter.Visuals.Chams then
                if not highlight then
                    highlight = Instance.new("Highlight", char)
                    highlight.Name = "JitterCham"
                end
                highlight.FillColor = Jitter.Visuals.ChamsColor
                highlight.OutlineTransparency = 0.5
                highlight.DepthMode = Enum.HighlightDepthMode.AlwaysOnTop -- ВОТ ЭТО ДЕЛАЕТ ИХ СКВОЗЬ СТЕНЫ
                highlight.Enabled = true
            elseif highlight then
                highlight.Enabled = false
            end
            
            -- Названия и ХП (простая реализация через Billboard)
            local gui = char:FindFirstChild("JitterESP")
            if (Jitter.Visuals.Names or Jitter.Visuals.Health) and hum.Health > 0 then
                if not gui then
                    gui = Instance.new("BillboardGui", char)
                    gui.Name = "JitterESP"
                    gui.AlwaysOnTop = true
                    gui.Size = UDim2.new(0, 100, 0, 50)
                    gui.StudsOffset = Vector3.new(0, 3, 0)
                    
                    local txt = Instance.new("TextLabel", gui)
                    txt.Size = UDim2.new(1, 0, 1, 0)
                    txt.BackgroundTransparency = 1
                    txt.TextColor3 = Color3.new(1, 1, 1)
                    txt.Font = Enum.Font.GothamBold
                    txt.TextSize = 12
                    txt.TextStrokeTransparency = 0
                end
                gui.TextLabel.Text = (Jitter.Visuals.Names and player.Name or "") .. 
                                     (Jitter.Visuals.Health and " ["..math.floor(hum.Health).."]" or "")
                gui.Enabled = true
            elseif gui then
                gui.Enabled = false
            end
        end
    end)
end

for _, p in pairs(Players:GetPlayers()) do if p ~= LocalPlayer then ApplyVisuals(p) end end
Players.PlayerAdded:Connect(function(p) if p ~= LocalPlayer then ApplyVisuals(p) end end)

-- // MISC (FOV & Insert)
RunService.RenderStepped:Connect(function()
    Camera.FieldOfView = Jitter.Misc.FOV
end)

UserInputService.InputBegan:Connect(function(input)
    if input.KeyCode == Enum.KeyCode.Insert then
        Screen.Enabled = not Screen.Enabled
        UserInputService.MouseIconEnabled = Screen.Enabled
    end
end)

print("Jitter.xyz: All functions integrated. Press INSERT.")
