class InputTextWithBtn extends React.Component {
  static propTypes = {
    no: PropTypes.string,
    label: PropTypes.string,
    name: PropTypes.string,
    value: PropTypes.string,
    btnName: PropTypes.string,
    feedback: PropTypes.string,
    responseResult: PropTypes.string,
    placeholder: PropTypes.string,
    isDisabled: PropTypes.bool,
    isDisabledTv: PropTypes.bool,
    onChange: PropTypes.func,
  }
  
  render() {
    if (!this.props.feedback && !this.props.responseResult)
      var cssInput = ''
    else if (this.props.feedback)
      var cssInput = 'is-invalid'
    else if (this.props.responseResult)
      var cssInput = 'is-valid'
    
    // 没有code or 有错误数据
    var isBtnDisabled = (this.props.feedback || this.props.isDisabled) ? true : false
    return(
      <div className="form-group row">
        <label className="col-5 col-sm-5 col-form-label col-form-label-sm">
          {this.props.label}
        </label>
        <div className="col-4 col-sm-4"> 
          <input 
            type="text"
            className={classNames('form-control form-control-sm', cssInput)}
            data-no={this.props.no}
            name={this.props.name}
            value={this.props.value}
            placeholder={this.props.placeholder}
            onChange={this.props.onChange} 
            disabled={this.props.isDisabledTv} />
        </div>
        <div className="col-3 col-sm-3">
          <button 
            type="button"
            className="btn btn-sm btn-block btn-sm btn-secondary"
            data-no={this.props.no}
            name={this.props.name}
            disabled={isBtnDisabled}
            onClick={this.props.onClick}>
              {this.props.btnName}
            </button>
        </div>
        <div className="col-5 col-sm-5"></div>
        <div className="col-7 col-sm-7">
          <div className="invalid-feedback d-block">
            {this.props.feedback}
          </div>
        </div>
      </div>
    )
  }
}