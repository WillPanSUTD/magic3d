#include "FaceFeatureRecognitionApp.h"
#include "../AppModules/MagicObjectManager.h"
#include "../AppModules/Face2DObj.h"
#include "../AppModules/FaceFeatureDetection.h"
#include "../AppModules/FaceDetection.h"
#include "../Tool/LogSystem.h"
#include "../Common/ToolKit.h"
#include <time.h>
#include <stdio.h>

namespace MagicApp
{
    FaceFeatureRecognitionApp::FaceFeatureRecognitionApp() :
        mMouseMode(MM_View),
        mpF2DObj(NULL),
        mpFfd(NULL),
        mpShapeRegression(NULL),
        mpFaceDetection(NULL)
    {
    }

    FaceFeatureRecognitionApp::~FaceFeatureRecognitionApp()
    {
    }

    bool FaceFeatureRecognitionApp::Enter(void)
    {
        InfoLog << "Enter FaceFeatureRecognition" << std::endl;
        mUI.Setup();
        SetupScene();

        return true;
    }

    bool FaceFeatureRecognitionApp::Update(float timeElapsed)
    {
        return true;
    }

    bool FaceFeatureRecognitionApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();

        return true;
    }

    bool FaceFeatureRecognitionApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        return true;
    }

    bool FaceFeatureRecognitionApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool FaceFeatureRecognitionApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        if (mMouseMode == MM_Mark_KeyPoint)
        {
            int hPos = arg.state.Y.abs - 10;
            int wPos = arg.state.X.abs - 110;
            SynthesizeData(wPos, hPos);
        }
        else if (mMouseMode == MM_Test_KeyPoint)
        {
            int hPos = arg.state.Y.abs - 10;
            int wPos = arg.state.X.abs - 110;
            std::vector<double> initPos(2);
            initPos.at(0) = hPos;
            initPos.at(1) = wPos;
            cv::Mat img = mpF2DObj->GetFaceImage();
            std::vector<double> finalPos;
            mpFfd->PoseRegression(img, initPos, finalPos);
            UpdateDisplayImage(&initPos, &finalPos);
            //UpdateDisplayImage(&initPos, NULL);
        }
        else if (mMouseMode == MM_Test_Shape)
        {
            int hPos = arg.state.Y.abs - 10;
            int wPos = arg.state.X.abs - 110;
            cv::Mat img = mpF2DObj->GetFaceImage();
            std::vector<double> meanFace = mpShapeRegression->GetMeanFace();
            int markSize = meanFace.size() / 2;
            for (int markId = 0; markId < markSize; markId++)
            {
                meanFace.at(markId * 2) += hPos;
                meanFace.at(markId * 2 + 1) += wPos;
            }
            std::vector<double> finalFace;
            mpShapeRegression->ShapeRegression(img, meanFace, finalFace);
            UpdateDisplayImage(NULL, &finalFace);
        }

        return true;
    }

    bool FaceFeatureRecognitionApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        if (arg.key == OIS::KC_K)
        {
            MarkKeyPoint();
        }
        else if (arg.key == OIS::KC_O)
        {
            OpenTestImage();
        }
        else if (arg.key == OIS::KC_S)
        {
            SaveCascadedRegression();
        }
        else if (arg.key == OIS::KC_L)
        {
            //LoadCascadedRegression();
            LoadShapeRegression();
        }
        else if (arg.key == OIS::KC_T)
        {
            //TestKeyPoint();
            TestShape();
        }
        else if (arg.key == OIS::KC_F)
        {
            //GenerateFacewareHouseFace();
            GenerateNonFace();
            //TestFaceDetection();
        }

        return true;
    }

    void FaceFeatureRecognitionApp::WindowResized( Ogre::RenderWindow* rw )
    {

    }

    void FaceFeatureRecognitionApp::SetupScene(void)
    {
        if (!(MOMGR->IsObjExist("Face2DObj")))
        {
            MOMGR->InsertObj("Face2DObj", new Face2DObj);
        }
        mpF2DObj = dynamic_cast<Face2DObj*>(MOMGR->GetObj("Face2DObj"));
        if (!(MOMGR->IsObjExist("CascadedFaceFeatureDetection")))
        {
            MOMGR->InsertObj("CascadedFaceFeatureDetection", new CascadedFaceFeatureDetection);
        }
        mpFfd = dynamic_cast<CascadedFaceFeatureDetection*>(MOMGR->GetObj("CascadedFaceFeatureDetection"));
        if (!(MOMGR->IsObjExist("ShapeRegression")))
        {
            MOMGR->InsertObj("ShapeRegression", new ShapeFaceFeatureDetection);
        }
        mpShapeRegression = dynamic_cast<ShapeFaceFeatureDetection*>(MOMGR->GetObj("ShapeRegression"));
        if (!(MOMGR->IsObjExist("FaceDetection")))
        {
            MOMGR->InsertObj("FaceDetection", new FaceDetection);
        }
        mpFaceDetection = dynamic_cast<FaceDetection*>(MOMGR->GetObj("FaceDetection"));
    }

    void FaceFeatureRecognitionApp::ShutdownScene(void)
    {

    }

    void FaceFeatureRecognitionApp::OpenImage(void)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.jpg)\0*.jpg\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpF2DObj->LoadFaceImage(fileName);
            std::string::size_type pos = fileName.rfind(".");
            fileName.replace(pos, 4, ".land");
            mpF2DObj->LoadLandFfp(fileName, mpF2DObj->GetFaceImage().rows);
            std::vector<double> fpsList;
            mpF2DObj->GetFfp()->GetFps(&fpsList);
            UpdateDisplayImage(NULL, &fpsList);
        }
    }

    void FaceFeatureRecognitionApp::LearnCascadedRegression(void)
    {
        std::string fileName;
        char filterName[] = "Land Files(*.txt)\0*.txt\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpFfd->LearnRegression(fileName);
            mpFfd->Save("./simple.cas");
        }
    }

    void FaceFeatureRecognitionApp::SaveCascadedRegression(void)
    {
        std::string fileName;
        char filterName[] = "Cascade Files(*.cas)\0*.cas\0";
        if (MagicCore::ToolKit::FileSaveDlg(fileName, filterName))
        {
            mpFfd->Save(fileName);
        }
    }

    void FaceFeatureRecognitionApp::LoadCascadedRegression(void)
    {
        std::string fileName;
        char filterName[] = "Cascade Files(*.cas)\0*.cas\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpFfd->Load(fileName);
        }
    }

    void FaceFeatureRecognitionApp::ShapeRegression(void)
    {
        std::string fileName;
        char filterName[] = "Land Files(*.txt)\0*.txt\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpShapeRegression->LearnRegression(fileName);
            mpShapeRegression->Save("./Regression.shape");
        }
    }

    void FaceFeatureRecognitionApp::LoadShapeRegression(void)
    {
        std::string fileName;
        char filterName[] = "Shape Files(*.shape)\0*.shape\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpShapeRegression->Load(fileName);
        }
    }

    void FaceFeatureRecognitionApp::RealTimeFaceDetection(void)
    {
        std::string faceFile;
        char faceFilterName[] = "face Files(*.txt)\0*.txt\0";
        if ( MagicCore::ToolKit::FileOpenDlg(faceFile, faceFilterName) )
        {
            std::string nonFaceFile;
            char nonFaceFilterName[] = "non-face Files(*.txt)\0*.txt\0";
            if ( MagicCore::ToolKit::FileOpenDlg(nonFaceFile, nonFaceFilterName) )
            {
                mpFaceDetection->LearnDetector(faceFile, nonFaceFile, FaceDetection::DM_Default);
                mpFaceDetection->Save("./FaceDetection.rfd");
            }
        }
    }

    void FaceFeatureRecognitionApp::RealTimeFaceLoading(void)
    {
        std::string fileName;
        char filterName[] = "Shape Files(*.shape)\0*.shape\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpFaceDetection->Load(fileName);
        }
    }

    void FaceFeatureRecognitionApp::TestFaceDetection(void)
    {
        cv::Mat img = mpF2DObj->GetFaceImage();
        std::vector<int> faces;
        int detectNum = mpFaceDetection->DetectFace(img, faces);
        if (detectNum > 0)
        {
            std::vector<double> marks;
            for (int detectId = 0; detectId < detectNum; detectId++)
            {
                int baseId = detectId * 4;
                int topRow = faces.at(baseId);
                int downRow =topRow + faces.at(baseId + 3);
                int leftCol = faces.at(baseId + 1);
                int rightCol = leftCol + faces.at(baseId + 2);
                for (int colId = leftCol; colId <= rightCol; colId++)
                {
                    marks.push_back(topRow);
                    marks.push_back(colId);
                    marks.push_back(downRow);
                    marks.push_back(colId);
                }
                for (int rowId = topRow; rowId <= downRow; rowId++)
                {
                    marks.push_back(rowId);
                    marks.push_back(leftCol);
                    marks.push_back(rowId);
                    marks.push_back(rightCol);
                }
            }
            UpdateDisplayImage(&marks, NULL);
        }
    }

    void FaceFeatureRecognitionApp::OpenTestImage(void)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.jpg)\0*.jpg\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mpF2DObj->LoadFaceImage(fileName);
            UpdateDisplayImage(NULL, NULL);
        }
    }

    void FaceFeatureRecognitionApp::MarkKeyPoint(void)
    {
        if (mMouseMode == MM_Mark_KeyPoint)
        {
            mMouseMode = MM_View;
        }
        else
        {
            mMouseMode = MM_Mark_KeyPoint;
        }
    }

    void FaceFeatureRecognitionApp::SynthesizeData(int keyX, int keyY)
    {
        std::string folderPath = "./Synthesis";
        int shiftSize = 10;
        int fileIndex = 0;
        std::ofstream layoutFout("./Synthesis/LandName.txt");
        layoutFout << (2 * shiftSize + 1) * (2 * shiftSize + 1) << std::endl;
        cv::Mat img = mpF2DObj->GetFaceImage().clone();
        int imgH = img.rows;
        int imgW = img.cols;
        for (int wid = -shiftSize; wid <= shiftSize; wid++)
        {
            for (int hid = -shiftSize; hid <= shiftSize; hid++)
            {
                layoutFout << "/test" << fileIndex << ".land" << std::endl;
                
                std::stringstream ss;
                ss << folderPath << "/test" << fileIndex << ".land";
                std::string landName;
                ss >> landName;
                ss.clear();
                std::ofstream landOut(landName);
                landOut << 1 << std::endl;
                landOut << wid + keyX << " " << imgH - (hid + keyY) << std::endl;
                landOut.close();

                ss << folderPath << "/test" << fileIndex << ".jpg";
                std::string imgName;
                ss >> imgName;
                ss.clear();
                cv::Mat shiftImg(imgH, imgW, CV_8UC3);
                for (int xid = 0; xid < imgW; xid++)
                {
                    for (int yid = 0; yid < imgH; yid++)
                    {
                        int wPos = xid + wid;
                        wPos = wPos < 0 ? 0 : (wPos > imgW - 1 ? imgW - 1 : wPos);
                        int hPos = yid + hid;
                        hPos = hPos < 0 ? 0 : (hPos > imgH - 1 ? imgH - 1 : hPos);
                        unsigned char* shiftPixel = shiftImg.ptr(hPos, wPos);
                        unsigned char* imgPixel = img.ptr(yid, xid);
                        shiftPixel[0] = imgPixel[0];
                        shiftPixel[1] = imgPixel[1];
                        shiftPixel[2] = imgPixel[2];
                    }
                }
                cv::imwrite(imgName, shiftImg);
                shiftImg.release();
                
                fileIndex++;
            }
        }
        layoutFout.close();
    }

    void FaceFeatureRecognitionApp::TestKeyPoint(void)
    {
        if (mMouseMode != MM_Test_KeyPoint)
        {
            mMouseMode = MM_Test_KeyPoint;
        }
        else
        {
            mMouseMode = MM_View;
        }
    }

    void FaceFeatureRecognitionApp::TestShape(void)
    {
        if (mMouseMode != MM_Test_Shape)
        {
            mMouseMode = MM_Test_Shape;
            if (!mpShapeRegression->IsMeanFaceExist())
            {
                std::string fileName;
                char filterName[] = "Land Files(*.txt)\0*.txt\0";
                if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
                {
                    mpShapeRegression->CalMeanFace(fileName);
                }
            }
        }
        else
        {
            mMouseMode = MM_View;
        }
    }

    void FaceFeatureRecognitionApp::GenerateNonFace(void)
    {
        std::string fileName;
        char filterName[] = "Land Files(*.txt)\0*.txt\0";
        MagicCore::ToolKit::FileOpenDlg(fileName, filterName);
        std::string imgPath = fileName;
        std::string::size_type pos = imgPath.rfind("/");
        if (pos == std::string::npos)
        {
            pos = imgPath.rfind("\\");
        }
        imgPath.erase(pos);
        imgPath += "/";
        std::ifstream fin(fileName);
        int dataSize;
        fin >> dataSize;
        int cropSize = 128;
        int outputSize = 64;
        cv::Size cvOutputSize(outputSize, outputSize);
        int imgNumPerData = 30;
        srand(time(NULL));
        for (int dataId = 0; dataId < dataSize; dataId++)
        {
            std::string imgName;
            fin >> imgName;
            imgName = imgPath + imgName;
            cv::Mat imgOrigin = cv::imread(imgName);
            pos = imgName.rfind(".");
            imgName.erase(pos);
            cv::Mat grayImg;
            cv::cvtColor(imgOrigin, grayImg, CV_BGR2GRAY);
            imgOrigin.release();
            int imgH = grayImg.rows;
            int hMax = imgH - cropSize;
            int imgW = grayImg.cols;
            int wMax = imgW - cropSize;
            int synImgNum = 0;
            while (synImgNum < imgNumPerData)
            {
                int sH = rand() % hMax;
                int sW = rand() % wMax;
                cv::Mat cropImg(cropSize, cropSize, CV_8UC1);
                for (int hid = 0; hid < cropSize; hid++)
                {
                    for (int wid = 0; wid < cropSize; wid++)
                    {
                        cropImg.ptr(hid, wid)[0] = grayImg.ptr(sH + hid, sW + wid)[0];
                    }
                }
                cv::Mat outputImg(cvOutputSize, CV_8UC1);
                cv::resize(cropImg, outputImg, cvOutputSize);
                cropImg.release();
                std::stringstream ss;
                ss << imgName << "_" << synImgNum << ".jpg";
                std::string outputName;
                ss >> outputName;
                cv::imwrite(outputName, outputImg);
                outputImg.release();
                synImgNum++;
            }
            grayImg.release();
        }
        fin.close();
        DebugLog << "GenerateNonFace Done" << std::endl; 
    }

    void FaceFeatureRecognitionApp::GenerateFacewareHouseFace(void)
    {
        std::string fileName;
        char filterName[] = "Land Files(*.txt)\0*.txt\0";
        MagicCore::ToolKit::FileOpenDlg(fileName, filterName);
        std::string landPath = fileName;
        std::string::size_type pos = landPath.rfind("/");
        if (pos == std::string::npos)
        {
            pos = landPath.rfind("\\");
        }
        landPath.erase(pos);
        std::ifstream fin(fileName);
        int dataSize;
        fin >> dataSize;
        //dataSize = 1;
        int imgH, imgW;
        int marginSize = 10;
        int outputSize = 64;
        cv::Size cvOutputSize(outputSize, outputSize);
        int sRow, sCol, len;
        for (int dataId = 0; dataId < dataSize; dataId++)
        {
            std::string featureName;
            fin >> featureName;
            featureName = landPath + featureName;
            std::string imgName = featureName;
            std::string::size_type pos = imgName.rfind(".");
            imgName.replace(pos, 5, "_gray.jpg");
            std::string faceName = featureName;
            pos = faceName.rfind(".");
            faceName.replace(pos, 5, "_face.jpg");

            cv::Mat img = cv::imread(imgName);
            imgH = img.rows;
            imgW = img.cols;
            
            std::ifstream landFin(featureName);
            int markCount;
            landFin >> markCount;
            int left = 1.0e10;
            int right = 0;
            int top = 1.0e10;
            int down = 0;
            for (int markId = 0; markId < markCount; markId++)
            {
                double row, col;
                landFin >> col >> row;
                row = imgH - row;
                if (row < top)
                {
                    top = row;
                }
                if (row > down)
                {
                    down = row;
                }
                if (col < left)
                {
                    left = col;
                }
                if (col > right)
                {
                    right = col;
                }
            }
            landFin.close();
            sRow = top - marginSize;
            sCol = left - marginSize;
            int w = right - left;
            int h = down - top;
            len = w > h ? w : h;
            len += marginSize * 2;
            cv::Mat cropImg(len, len, CV_8UC1);
            for (int hid = 0; hid < len; hid++)
            {
                for (int wid = 0; wid < len; wid++)
                {
                    cropImg.ptr(hid, wid)[0] = img.ptr(sRow + hid, sCol + wid)[0];
                }
            }
            cv::Mat outputImg(cvOutputSize, CV_8UC1);
            cv::resize(cropImg, outputImg, cvOutputSize);
            cv::imwrite(faceName, outputImg);
            cropImg.release();
            img.release();
            outputImg.release();
        }
        fin.close();
        DebugLog << "GenerateFacewareHouseFace Done" << std::endl;
    }

    void FaceFeatureRecognitionApp::UpdateDisplayImage(const std::vector<double>* dpsList, const std::vector<double>* fpsList)
    {
        cv::Mat displayImg = mpF2DObj->GetFaceImage().clone();
        MarkPointsToImage(displayImg, dpsList, 0, 255, 255, 1);
        MarkPointsToImage(displayImg, fpsList, 255, 255, 0, 1);
        mUI.UpdateImage(displayImg);
        displayImg.release();
    }

    void FaceFeatureRecognitionApp::MarkPointsToImage(cv::Mat& img, const std::vector<double>* markList, 
            unsigned char blue, unsigned char green, unsigned char red, int markWidth)
    {
        if (markList != NULL)
        {
            int imgW = img.cols;
            int imgH = img.rows;
            int markNum = markList->size() / 2;
            for (int mid = 0; mid < markNum; mid++)
            {
                int wPos = floor(markList->at(2 * mid + 1) + 0.5);
                int hPos = floor(markList->at(2 * mid) + 0.5);
                int hBottom = hPos - markWidth;
                hBottom = hBottom > 0 ? hBottom : 0;
                int hUp = hPos + markWidth;
                hUp = hUp >= imgH ? imgH - 1 : hUp;
                int wLeft = wPos - markWidth;
                wLeft = wLeft > 0 ? wLeft : 0;
                int wRight = wPos + markWidth;
                wRight = wRight >= imgW ? imgW - 1 : wRight;
                for (int hid = hBottom; hid <= hUp; hid++)
                {
                    for (int wid = wLeft; wid <= wRight; wid++)
                    {
                        unsigned char* pixel = img.ptr(hid, wid);
                        pixel[0] = blue;
                        pixel[1] = green;
                        pixel[2] = red;
                    }
                }
            }
        }
    }
}