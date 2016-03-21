#ifndef _CAESARIA_FONT_CONTEXT_H_INCLUDE_
#define _CAESARIA_FONT_CONTEXT_H_INCLUDE_

#include "litehtml.h"
#include "font/font.hpp"
#include "gfx/engine.hpp"

namespace html
{

struct CntxClipbox
{
  typedef std::vector<CntxClipbox> vector;
  litehtml::position	box;
  litehtml::border_radiuses radius;

  CntxClipbox(const litehtml::position& vBox, litehtml::border_radiuses vRad)
  {
    box = vBox;
    radius = vRad;
  }

  CntxClipbox(const CntxClipbox& val)
  {
    box = val.box;
    radius = val.radius;
  }
  CntxClipbox& operator=(const CntxClipbox& val)
  {
    box = val.box;
    radius = val.radius;
    return *this;
  }
};

struct CntxFont
{
  Font	font;
  int		size;
  bool	underline;
  bool	strikeout;
};

class RenderContext :	public litehtml::document_container
{
  typedef std::map<litehtml::tstring, gfx::Picture>	ImagesMap;

protected:
  gfx::Picture _picture;
  NColor _color;
  ImagesMap					m_images;
  CntxClipbox::vector		m_clips;
public:
  RenderContext(int width,int height);
  virtual ~RenderContext(void);

  gfx::Picture picture() { return _picture; }

  virtual void set_caption(const litehtml::tchar_t *caption);
  virtual void set_base_url(const litehtml::tchar_t *base_url);
  virtual void on_anchor_click(const litehtml::tchar_t *url, const litehtml::element::ptr &el);
  virtual void set_cursor(const litehtml::tchar_t *cursor);
  virtual void import_css(litehtml::tstring &text, const litehtml::tstring &url, litehtml::tstring &baseurl);
  virtual void get_client_rect(litehtml::position &client) const;
  virtual litehtml::uint_ptr create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) override;
  virtual void delete_font(litehtml::uint_ptr hFont) override;
  virtual int	 text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont) override;
  virtual void draw_text(litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override;
  virtual int	 pt_to_px(int pt) override;
  virtual int	 get_default_font_size() const override;
  virtual const litehtml::tchar_t*	get_default_font_name() const override;
  virtual void load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready) override;
  virtual void get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz) override;
  virtual void draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg) override;
  virtual void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) override;
  virtual void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;
  virtual std::shared_ptr<litehtml::element>	create_element(const litehtml::tchar_t *tag_name,
  const litehtml::string_map &attributes,
  const std::shared_ptr<litehtml::document> &doc) override;
  virtual void get_media_features(litehtml::media_features& media) const override;
  virtual void get_language(litehtml::tstring& language, litehtml::tstring & culture) const override;
  virtual void link(const std::shared_ptr<litehtml::document> &ptr, const litehtml::element::ptr& el) override;


  virtual	void transform_text(litehtml::tstring& text, litehtml::text_transform tt) override;
  virtual void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y) override;
  virtual void del_clip() override;

  virtual void make_url( const litehtml::tchar_t* url, const litehtml::tchar_t* basepath, litehtml::tstring& out );
  virtual gfx::Picture get_image(const litehtml::tchar_t* url, bool redraw_on_ready);

  void clear_images();

protected:
  virtual void draw_ellipse(gfx::Picture* cr, int x, int y, int width, int height, const litehtml::web_color& color, int line_width);
  virtual void fill_ellipse(gfx::Picture* cr, int x, int y, int width, int height, const litehtml::web_color& color);
  virtual void rounded_rectangle(gfx::Picture* cr, const litehtml::position &pos, const litehtml::border_radiuses &radius );

private:
  void			apply_clip(gfx::Picture* cr);
  void			add_path_arc(gfx::Picture* cr, double x, double y, double rx, double ry, double a1, double a2, bool neg);
  void			set_color(gfx::Picture* cr, litehtml::web_color color);
  void			draw_pixbuf(gfx::Picture* cr, const gfx::Picture& bmp, int x, int y, int cx, int cy);
  uint32_t* surface_from_pixbuf(const gfx::Picture& bmp);
};

}//end namespace html

#endif //_CAESARIA_FONT_CONTEXT_H_INCLUDE_
